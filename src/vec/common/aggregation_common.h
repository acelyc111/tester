#pragma once

#include <array>

#include <vec/common/sip_hash.h>
#include <vec/common/arena.h>
#include <vec/common/uint128.h>
#include <vec/common/hash_table/hash.h>
#include <vec/common/memcpy_small.h>
#include <vec/common/assert_cast.h>
#include <vec/core/defines.h>
#include <vec/common/string_ref.h>
#include <vec/columns/column.h>
#include <vec/columns/columns_number.h>
// #include <vec/columns/columnFixedString.h>
// #include <vec/columns/columnLowCardinality.h>


template <>
struct DefaultHash<StringRef> : public StringRefHash {};


namespace doris::vectorized
{

using Sizes = std::vector<size_t>;

/// When packing the values of nullable columns at a given row, we have to
/// store the fact that these values are nullable or not. This is achieved
/// by encoding this information as a bitmap. Let S be the size in bytes of
/// a packed values binary blob and T the number of bytes we may place into
/// this blob, the size that the bitmap shall occupy in the blob is equal to:
/// ceil(T/8). Thus we must have: S = T + ceil(T/8). Below we indicate for
/// each value of S, the corresponding value of T, and the bitmap size:
///
/// 32,28,4
/// 16,14,2
/// 8,7,1
/// 4,3,1
/// 2,1,1
///

namespace
{

template <typename T>
constexpr auto getBitmapSize()
{
    return
        (sizeof(T) == 32) ?
            4 :
        (sizeof(T) == 16) ?
            2 :
        ((sizeof(T) == 8) ?
            1 :
        ((sizeof(T) == 4) ?
            1 :
        ((sizeof(T) == 2) ?
            1 :
        0)));
}

}

template <typename T>
using KeysNullMap = std::array<UInt8, getBitmapSize<T>()>;

/// Pack into a binary blob of type T a set of fixed-size keys. Granted that all the keys fit into the
/// binary blob, they are disposed in it consecutively.
template <typename T, bool has_low_cardinality = false>
static inline T ALWAYS_INLINE packFixed(
    size_t i, size_t keys_size, const ColumnRawPtrs & key_columns, const Sizes & key_sizes,
    const ColumnRawPtrs * low_cardinality_positions [[maybe_unused]] = nullptr,
    const Sizes * low_cardinality_sizes [[maybe_unused]] = nullptr)
{
    union
    {
        T key;
        char bytes[sizeof(key)] = {};
    };

    size_t offset = 0;

    for (size_t j = 0; j < keys_size; ++j)
    {
        size_t index = i;
        const IColumn * column = key_columns[j];
        if constexpr (has_low_cardinality)
        {
            if (const IColumn * positions = (*low_cardinality_positions)[j])
            {
                switch ((*low_cardinality_sizes)[j])
                {
                    case sizeof(UInt8): index = assert_cast<const ColumnUInt8 *>(positions)->getElement(i); break;
                    case sizeof(UInt16): index = assert_cast<const ColumnUInt16 *>(positions)->getElement(i); break;
                    case sizeof(UInt32): index = assert_cast<const ColumnUInt32 *>(positions)->getElement(i); break;
                    case sizeof(UInt64): index = assert_cast<const ColumnUInt64 *>(positions)->getElement(i); break;
                    default: throw Exception("Unexpected size of index type for low cardinality column.", ErrorCodes::LOGICAL_ERROR);
                }
            }
        }

        switch (key_sizes[j])
        {
            case 1:
                memcpy(bytes + offset, static_cast<const ColumnVectorHelper *>(column)->getRawDataBegin<1>() + index, 1);
                offset += 1;
                break;
            case 2:
                memcpy(bytes + offset, static_cast<const ColumnVectorHelper *>(column)->getRawDataBegin<2>() + index * 2, 2);
                offset += 2;
                break;
            case 4:
                memcpy(bytes + offset, static_cast<const ColumnVectorHelper *>(column)->getRawDataBegin<4>() + index * 4, 4);
                offset += 4;
                break;
            case 8:
                memcpy(bytes + offset, static_cast<const ColumnVectorHelper *>(column)->getRawDataBegin<8>() + index * 8, 8);
                offset += 8;
                break;
            default:
                memcpy(bytes + offset, static_cast<const ColumnVectorHelper *>(column)->getRawDataBegin<1>() + index * key_sizes[j], key_sizes[j]);
                offset += key_sizes[j];
        }
    }

    return key;
}

/// Similar as above but supports nullable values.
template <typename T>
static inline T ALWAYS_INLINE packFixed(
    size_t i, size_t keys_size, const ColumnRawPtrs & key_columns, const Sizes & key_sizes,
    const KeysNullMap<T> & bitmap)
{
    union
    {
        T key;
        char bytes[sizeof(key)] = {};
    };

    size_t offset = 0;

    static constexpr auto bitmap_size = std::tuple_size<KeysNullMap<T>>::value;
    static constexpr bool has_bitmap = bitmap_size > 0;

    if (has_bitmap)
    {
        memcpy(bytes + offset, bitmap.data(), bitmap_size * sizeof(UInt8));
        offset += bitmap_size;
    }

    for (size_t j = 0; j < keys_size; ++j)
    {
        bool is_null;

        if (!has_bitmap)
            is_null = false;
        else
        {
            size_t bucket = j / 8;
            size_t off = j % 8;
            is_null = ((bitmap[bucket] >> off) & 1) == 1;
        }

        if (is_null)
            continue;

        switch (key_sizes[j])
        {
            case 1:
                memcpy(bytes + offset, static_cast<const ColumnVectorHelper *>(key_columns[j])->getRawDataBegin<1>() + i, 1);
                offset += 1;
                break;
            case 2:
                memcpy(bytes + offset, static_cast<const ColumnVectorHelper *>(key_columns[j])->getRawDataBegin<2>() + i * 2, 2);
                offset += 2;
                break;
            case 4:
                memcpy(bytes + offset, static_cast<const ColumnVectorHelper *>(key_columns[j])->getRawDataBegin<4>() + i * 4, 4);
                offset += 4;
                break;
            case 8:
                memcpy(bytes + offset, static_cast<const ColumnVectorHelper *>(key_columns[j])->getRawDataBegin<8>() + i * 8, 8);
                offset += 8;
                break;
            default:
                memcpy(bytes + offset, static_cast<const ColumnVectorHelper *>(key_columns[j])->getRawDataBegin<1>() + i * key_sizes[j], key_sizes[j]);
                offset += key_sizes[j];
        }
    }

    return key;
}


/// Hash a set of keys into a UInt128 value.
static inline UInt128 ALWAYS_INLINE hash128(
    size_t i, size_t keys_size, const ColumnRawPtrs & key_columns)
{
    UInt128 key;
    SipHash hash;

    for (size_t j = 0; j < keys_size; ++j)
        key_columns[j]->updateHashWithValue(i, hash);

    hash.get128(key.low, key.high);

    return key;
}


/// Copy keys to the pool. Then put into pool StringRefs to them and return the pointer to the first.
static inline StringRef * ALWAYS_INLINE placeKeysInPool(
    size_t keys_size, StringRefs & keys, Arena & pool)
{
    for (size_t j = 0; j < keys_size; ++j)
    {
        char * place = pool.alloc(keys[j].size);
        memcpySmallAllowReadWriteOverflow15(place, keys[j].data, keys[j].size);
        keys[j].data = place;
    }

    /// Place the StringRefs on the newly copied keys in the pool.
    char * res = pool.alignedAlloc(keys_size * sizeof(StringRef), alignof(StringRef));
    memcpySmallAllowReadWriteOverflow15(res, keys.data(), keys_size * sizeof(StringRef));

    return reinterpret_cast<StringRef *>(res);
}


/** Serialize keys into a continuous chunk of memory.
  */
static inline StringRef ALWAYS_INLINE serializeKeysToPoolContiguous(
    size_t i, size_t keys_size, const ColumnRawPtrs & key_columns, Arena & pool)
{
    const char * begin = nullptr;

    size_t sum_size = 0;
    for (size_t j = 0; j < keys_size; ++j)
        sum_size += key_columns[j]->serializeValueIntoArena(i, pool, begin).size;

    return {begin, sum_size};
}


}
