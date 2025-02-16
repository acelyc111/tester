// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#include "vec/data_types/data_type_number_base.h"

#include <type_traits>

#include "vec/columns/column_const.h"
#include "vec/columns/column_vector.h"
// #include <IO/ReadHelpers.h>
// #include <IO/WriteHelpers.h>
#include "vec/common/assert_cast.h"
#include "vec/common/nan_utils.h"
#include "vec/common/typeid_cast.h"
// #include <Formats/FormatSettings.h>
// #include <Formats/ProtobufReader.h>
// #include <Formats/ProtobufWriter.h>

namespace doris::vectorized {

template <typename T>
void DataTypeNumberBase<T>::to_string(const IColumn& column, size_t row_num,
                                      BufferWritable& ostr) const {
    if constexpr (std::is_same<T, __int128_t>::value || std::is_same<T, UInt128>::value) {
        // write int 128
    } else if constexpr (std::is_integral<T>::value || std::numeric_limits<T>::is_iec559) {
        ostr.write_number(assert_cast<const ColumnVector<T>&>(column).getData()[row_num]);
    }
}

// template <typename T>
// void DataTypeNumberBase<T>::serializeText(const IColumn & column, size_t row_num, WriteBuffer & ostr, const FormatSettings &) const
// {
//     writeText(assert_cast<const ColumnVector<T> &>(column).getData()[row_num], ostr);
// }

// template <typename T>
// void DataTypeNumberBase<T>::deserializeText(IColumn & column, ReadBuffer & istr, const FormatSettings &) const
// {
//     T x;

//     if constexpr (std::is_integral_v<T> && std::is_arithmetic_v<T>)
//         readIntTextUnsafe(x, istr);
//     else
//         readText(x, istr);

//     assert_cast<ColumnVector<T> &>(column).getData().push_back(x);
// }

// template <typename T>
// static inline void writeDenormalNumber(T x, WriteBuffer & ostr)
// {
//     if constexpr (std::is_floating_point_v<T>)
//     {
//         if (std::signbit(x))
//         {
//             if (isNaN(x))
//                 writeCString("-nan", ostr);
//             else
//                 writeCString("-inf", ostr);
//         }
//         else
//         {
//             if (isNaN(x))
//                 writeCString("nan", ostr);
//             else
//                 writeCString("inf", ostr);
//         }
//     }
//     else
//     {
//         /// This function is not called for non floating point numbers.
//         (void)x;
//     }
// }

// template <typename T>
// void DataTypeNumberBase<T>::serializeTextJSON(const IColumn & column, size_t row_num, WriteBuffer & ostr, const FormatSettings & settings) const
// {
//     auto x = assert_cast<const ColumnVector<T> &>(column).getData()[row_num];
//     bool is_finite = isFinite(x);

//     const bool need_quote = (std::is_integral_v<T> && (sizeof(T) == 8) && settings.json.quote_64bit_integers)
//         || (settings.json.quote_denormals && !is_finite);

//     if (need_quote)
//         writeChar('"', ostr);

//     if (is_finite)
//         writeText(x, ostr);
//     else if (!settings.json.quote_denormals)
//         writeCString("null", ostr);
//     else
//         writeDenormalNumber(x, ostr);

//     if (need_quote)
//         writeChar('"', ostr);
// }

// template <typename T>
// void DataTypeNumberBase<T>::deserializeTextJSON(IColumn & column, ReadBuffer & istr, const FormatSettings &) const
// {
//     bool has_quote = false;
//     if (!istr.eof() && *istr.position() == '"')        /// We understand the number both in quotes and without.
//     {
//         has_quote = true;
//         ++istr.position();
//     }

//     FieldType x;

//     /// null
//     if (!has_quote && !istr.eof() && *istr.position() == 'n')
//     {
//         ++istr.position();
//         assertString("ull", istr);

//         x = NaNOrZero<T>();
//     }
//     else
//     {
//         static constexpr bool is_uint8 = std::is_same_v<T, UInt8>;
//         static constexpr bool is_int8 = std::is_same_v<T, Int8>;

//         if (is_uint8 || is_int8)
//         {
//             // extra conditions to parse true/false strings into 1/0
//             if (istr.eof())
//                 throwReadAfterEOF();
//             if (*istr.position() == 't' || *istr.position() == 'f')
//             {
//                 bool tmp = false;
//                 readBoolTextWord(tmp, istr);
//                 x = tmp;
//             }
//             else
//                 readText(x, istr);
//         }
//         else
//         {
//             readText(x, istr);
//         }

//         if (has_quote)
//             assertChar('"', istr);
//     }

//     assert_cast<ColumnVector<T> &>(column).getData().push_back(x);
// }

// template <typename T>
// void DataTypeNumberBase<T>::deserializeTextCSV(IColumn & column, ReadBuffer & istr, const FormatSettings &) const
// {
//     FieldType x;
//     readCSV(x, istr);
//     assert_cast<ColumnVector<T> &>(column).getData().push_back(x);
// }

template <typename T>
Field DataTypeNumberBase<T>::getDefault() const {
    return NearestFieldType<FieldType>();
}

// template <typename T>
// void DataTypeNumberBase<T>::serializeBinary(const Field & field, WriteBuffer & ostr) const
// {
//     /// ColumnVector<T>::value_type is a narrower type. For example, UInt8, when the Field type is UInt64
//     typename ColumnVector<T>::value_type x = get<NearestFieldType<FieldType>>(field);
//     writeBinary(x, ostr);
// }

// template <typename T>
// void DataTypeNumberBase<T>::deserializeBinary(Field & field, ReadBuffer & istr) const
// {
//     typename ColumnVector<T>::value_type x;
//     readBinary(x, istr);
//     field = NearestFieldType<FieldType>(x);
// }

// template <typename T>
// void DataTypeNumberBase<T>::serializeBinary(const IColumn & column, size_t row_num, WriteBuffer & ostr) const
// {
//     writeBinary(assert_cast<const ColumnVector<T> &>(column).getData()[row_num], ostr);
// }

// template <typename T>
// void DataTypeNumberBase<T>::deserializeBinary(IColumn & column, ReadBuffer & istr) const
// {
//     typename ColumnVector<T>::value_type x;
//     readBinary(x, istr);
//     assert_cast<ColumnVector<T> &>(column).getData().push_back(x);
// }

// template <typename T>
// void DataTypeNumberBase<T>::serializeBinaryBulk(const IColumn & column, WriteBuffer & ostr, size_t offset, size_t limit) const
// {
//     const typename ColumnVector<T>::Container & x = typeid_cast<const ColumnVector<T> &>(column).getData();

//     size_t size = x.size();

//     if (limit == 0 || offset + limit > size)
//         limit = size - offset;

//     if (limit)
//         ostr.write(reinterpret_cast<const char *>(&x[offset]), sizeof(typename ColumnVector<T>::value_type) * limit);
// }

// template <typename T>
// void DataTypeNumberBase<T>::deserializeBinaryBulk(IColumn & column, ReadBuffer & istr, size_t limit, double /*avg_value_size_hint*/) const
// {
//     typename ColumnVector<T>::Container & x = typeid_cast<ColumnVector<T> &>(column).getData();
//     size_t initial_size = x.size();
//     x.resize(initial_size + limit);
//     size_t size = istr.readBig(reinterpret_cast<char*>(&x[initial_size]), sizeof(typename ColumnVector<T>::value_type) * limit);
//     x.resize(initial_size + size / sizeof(typename ColumnVector<T>::value_type));
// }

// template <typename T>
// void DataTypeNumberBase<T>::serializeProtobuf(const IColumn & column, size_t row_num, ProtobufWriter & protobuf, size_t & value_index) const
// {
//     if (value_index)
//         return;
//     value_index = static_cast<bool>(protobuf.writeNumber(assert_cast<const ColumnVector<T> &>(column).getData()[row_num]));
// }

// template <typename T>
// void DataTypeNumberBase<T>::deserializeProtobuf(IColumn & column, ProtobufReader & protobuf, bool allow_add_row, bool & row_added) const
// {
//     row_added = false;
//     T value;
//     if (!protobuf.readNumber(value))
//         return;

//     auto & container = typeid_cast<ColumnVector<T> &>(column).getData();
//     if (allow_add_row)
//     {
//         container.emplace_back(value);
//         row_added = true;
//     }
//     else
//         container.back() = value;
// }

template <typename T>
MutableColumnPtr DataTypeNumberBase<T>::createColumn() const {
    return ColumnVector<T>::create();
}

template <typename T>
bool DataTypeNumberBase<T>::isValueRepresentedByInteger() const {
    return std::is_integral_v<T>;
}

template <typename T>
bool DataTypeNumberBase<T>::isValueRepresentedByUnsignedInteger() const {
    return std::is_integral_v<T> && std::is_unsigned_v<T>;
}

/// Explicit template instantiations - to avoid code bloat in headers.
template class DataTypeNumberBase<UInt8>;
template class DataTypeNumberBase<UInt16>;
template class DataTypeNumberBase<UInt32>;
template class DataTypeNumberBase<UInt64>;
template class DataTypeNumberBase<UInt128>;
template class DataTypeNumberBase<Int8>;
template class DataTypeNumberBase<Int16>;
template class DataTypeNumberBase<Int32>;
template class DataTypeNumberBase<Int64>;
template class DataTypeNumberBase<Int128>;
template class DataTypeNumberBase<Float32>;
template class DataTypeNumberBase<Float64>;

} // namespace doris::vectorized
