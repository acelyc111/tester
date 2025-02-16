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

#include "vec/data_types/data_types_decimal.h"

#include <type_traits>

#include "vec/common/assert_cast.h"
#include "vec/common/int_exp.h"
#include "vec/common/typeid_cast.h"
//#include <DataTypes/DataTypeFactory.h>
//#include <Formats/ProtobufReader.h>
//#include <Formats/ProtobufWriter.h>
//#include <IO/ReadHelpers.h>
//#include <IO/WriteHelpers.h>
//#include <IO/readDecimalText.h>
//#include <Parsers/IAST.h>
//#include <Parsers/ASTLiteral.h>
//#include <Interpreters/Context.h>

namespace doris::vectorized {

namespace ErrorCodes {
extern const int NUMBER_OF_ARGUMENTS_DOESNT_MATCH;
extern const int ILLEGAL_TYPE_OF_ARGUMENT;
extern const int ARGUMENT_OUT_OF_BOUND;
} // namespace ErrorCodes

//bool decimalCheckComparisonOverflow(const Context & context) { return context.getSettingsRef().decimal_check_overflow; }
//bool decimalCheckArithmeticOverflow(const Context & context) { return context.getSettingsRef().decimal_check_overflow; }

//

template <typename T>
std::string DataTypeDecimal<T>::doGetName() const {
    std::stringstream ss;
    ss << "Decimal(" << precision << ", " << scale << ")";
    return ss.str();
}

template <typename T>
bool DataTypeDecimal<T>::equals(const IDataType& rhs) const {
    if (auto* ptype = typeid_cast<const DataTypeDecimal<T>*>(&rhs))
        return scale == ptype->getScale();
    return false;
}

//template <typename T>
//void DataTypeDecimal<T>::serializeText(const IColumn & column, size_t row_num, WriteBuffer & ostr, const FormatSettings &) const
//{
//    T value = assert_cast<const ColumnType &>(column).getData()[row_num];
//    writeText(value, scale, ostr);
//}

//template <typename T>
//bool DataTypeDecimal<T>::tryReadText(T & x, ReadBuffer & istr, UInt32 precision, UInt32 scale)
//{
//    UInt32 unread_scale = scale;
//    bool done = tryReadDecimalText(istr, x, precision, unread_scale);
//    x *= getScaleMultiplier(unread_scale);
//    return done;
//}

//template <typename T>
//void DataTypeDecimal<T>::readText(T & x, ReadBuffer & istr, UInt32 precision, UInt32 scale, bool csv)
//{
//    UInt32 unread_scale = scale;
//    if (csv)
//        readCSVDecimalText(istr, x, precision, unread_scale);
//    else
//        readDecimalText(istr, x, precision, unread_scale);
//    x *= getScaleMultiplier(unread_scale);
//}
//
//template <typename T>
//void DataTypeDecimal<T>::deserializeText(IColumn & column, ReadBuffer & istr, const FormatSettings &) const
//{
//    T x;
//    readText(x, istr);
//    assert_cast<ColumnType &>(column).getData().push_back(x);
//}

//template <typename T>
//void DataTypeDecimal<T>::deserializeTextCSV(IColumn & column, ReadBuffer & istr, const FormatSettings &) const
//{
//    T x;
//    readText(x, istr, true);
//    assert_cast<ColumnType &>(column).getData().push_back(x);
//}

//template <typename T>
//T DataTypeDecimal<T>::parseFromString(const String & str) const
//{
//    ReadBufferFromMemory buf(str.data(), str.size());
//    T x;
//    UInt32 unread_scale = scale;
//    readDecimalText(buf, x, precision, unread_scale, true);
//    x *= getScaleMultiplier(unread_scale);
//    return x;
//}

//template <typename T>
//void DataTypeDecimal<T>::serializeBinary(const Field & field, WriteBuffer & ostr) const
//{
//    FieldType x = get<DecimalField<T>>(field);
//    writeBinary(x, ostr);
//}
//
//template <typename T>
//void DataTypeDecimal<T>::serializeBinary(const IColumn & column, size_t row_num, WriteBuffer & ostr) const
//{
//    const FieldType & x = assert_cast<const ColumnType &>(column).getElement(row_num);
//    writeBinary(x, ostr);
//}
//
//template <typename T>
//void DataTypeDecimal<T>::serializeBinaryBulk(const IColumn & column, WriteBuffer & ostr, size_t offset, size_t limit) const
//{
//    const typename ColumnType::Container & x = typeid_cast<const ColumnType &>(column).getData();
//
//    size_t size = x.size();
//
//    if (limit == 0 || offset + limit > size)
//        limit = size - offset;
//
//    ostr.write(reinterpret_cast<const char *>(&x[offset]), sizeof(FieldType) * limit);
//}

//template <typename T>
//void DataTypeDecimal<T>::deserializeBinary(Field & field, ReadBuffer & istr) const
//{
//    typename FieldType::NativeType x;
//    readBinary(x, istr);
//    field = DecimalField(T(x), scale);
//}
//
//template <typename T>
//void DataTypeDecimal<T>::deserializeBinary(IColumn & column, ReadBuffer & istr) const
//{
//    typename FieldType::NativeType x;
//    readBinary(x, istr);
//    assert_cast<ColumnType &>(column).getData().push_back(FieldType(x));
//}
//
//template <typename T>
//void DataTypeDecimal<T>::deserializeBinaryBulk(IColumn & column, ReadBuffer & istr, size_t limit, double) const
//{
//    typename ColumnType::Container & x = typeid_cast<ColumnType &>(column).getData();
//    size_t initial_size = x.size();
//    x.resize(initial_size + limit);
//    size_t size = istr.readBig(reinterpret_cast<char*>(&x[initial_size]), sizeof(FieldType) * limit);
//    x.resize(initial_size + size / sizeof(FieldType));
//}

//template <typename T>
//void DataTypeDecimal<T>::serializeProtobuf(const IColumn & column, size_t row_num, ProtobufWriter & protobuf, size_t & value_index) const
//{
//    if (value_index)
//        return;
//    value_index = static_cast<bool>(protobuf.writeDecimal(assert_cast<const ColumnType &>(column).getData()[row_num], scale));
//}
//

//template <typename T>
//void DataTypeDecimal<T>::deserializeProtobuf(IColumn & column, ProtobufReader & protobuf, bool allow_add_row, bool & row_added) const
//{
//    row_added = false;
//    T decimal;
//    if (!protobuf.readDecimal(decimal, precision, scale))
//        return;
//
//    auto & container = assert_cast<ColumnType &>(column).getData();
//    if (allow_add_row)
//    {
//        container.emplace_back(decimal);
//        row_added = true;
//    }
//    else
//        container.back() = decimal;
//}

template <typename T>
Field DataTypeDecimal<T>::getDefault() const {
    return DecimalField(T(0), scale);
}

template <typename T>
DataTypePtr DataTypeDecimal<T>::promoteNumericType() const {
    using PromotedType = DataTypeDecimal<Decimal128>;
    return std::make_shared<PromotedType>(PromotedType::maxPrecision(), scale);
}

template <typename T>
MutableColumnPtr DataTypeDecimal<T>::createColumn() const {
    return ColumnType::create(0, scale);
}

//

DataTypePtr createDecimal(UInt64 precision_value, UInt64 scale_value) {
    if (precision_value < minDecimalPrecision() ||
        precision_value > maxDecimalPrecision<Decimal128>())
        throw Exception("Wrong precision", ErrorCodes::ARGUMENT_OUT_OF_BOUND);

    if (static_cast<UInt64>(scale_value) > precision_value)
        throw Exception("Negative scales and scales larger than precision are not supported",
                        ErrorCodes::ARGUMENT_OUT_OF_BOUND);

    if (precision_value <= maxDecimalPrecision<Decimal32>())
        return std::make_shared<DataTypeDecimal<Decimal32>>(precision_value, scale_value);
    else if (precision_value <= maxDecimalPrecision<Decimal64>())
        return std::make_shared<DataTypeDecimal<Decimal64>>(precision_value, scale_value);
    return std::make_shared<DataTypeDecimal<Decimal128>>(precision_value, scale_value);
}

//static DataTypePtr create(const ASTPtr & arguments)
//{
//    if (!arguments || arguments->children.size() != 2)
//        throw Exception("Decimal data type family must have exactly two arguments: precision and scale",
//                        ErrorCodes::NUMBER_OF_ARGUMENTS_DOESNT_MATCH);
//
//    const auto * precision = arguments->children[0]->as<ASTLiteral>();
//    const auto * scale = arguments->children[1]->as<ASTLiteral>();
//
//    if (!precision || precision->value.getType() != Field::Types::UInt64 ||
//        !scale || !(scale->value.getType() == Field::Types::Int64 || scale->value.getType() == Field::Types::UInt64))
//        throw Exception("Decimal data type family must have a two numbers as its arguments", ErrorCodes::ILLEGAL_TYPE_OF_ARGUMENT);
//
//    UInt64 precision_value = precision->value.get<UInt64>();
//    UInt64 scale_value = scale->value.get<UInt64>();
//
//    return createDecimal(precision_value, scale_value);
//}
//
//template <typename T>
//static DataTypePtr createExact(const ASTPtr & arguments)
//{
//    if (!arguments || arguments->children.size() != 1)
//        throw Exception("Decimal data type family must have exactly two arguments: precision and scale",
//                        ErrorCodes::NUMBER_OF_ARGUMENTS_DOESNT_MATCH);
//
//    const auto * scale_arg = arguments->children[0]->as<ASTLiteral>();
//
//    if (!scale_arg || !(scale_arg->value.getType() == Field::Types::Int64 || scale_arg->value.getType() == Field::Types::UInt64))
//        throw Exception("Decimal data type family must have a two numbers as its arguments", ErrorCodes::ILLEGAL_TYPE_OF_ARGUMENT);
//
//    UInt64 precision = maxDecimalPrecision<T>();
//    UInt64 scale = scale_arg->value.get<UInt64>();
//
//    return createDecimal(precision, scale);
//}
//
//void registerDataTypeDecimal(DataTypeFactory & factory)
//{
//    factory.registerDataType("Decimal32", createExact<Decimal32>, DataTypeFactory::CaseInsensitive);
//    factory.registerDataType("Decimal64", createExact<Decimal64>, DataTypeFactory::CaseInsensitive);
//    factory.registerDataType("Decimal128", createExact<Decimal128>, DataTypeFactory::CaseInsensitive);
//
//    factory.registerDataType("Decimal", create, DataTypeFactory::CaseInsensitive);
//    factory.registerAlias("DEC", "Decimal", DataTypeFactory::CaseInsensitive);
//}

template <>
Decimal32 DataTypeDecimal<Decimal32>::getScaleMultiplier(UInt32 scale_) {
    return common::exp10_i32(scale_);
}

template <>
Decimal64 DataTypeDecimal<Decimal64>::getScaleMultiplier(UInt32 scale_) {
    return common::exp10_i64(scale_);
}

template <>
Decimal128 DataTypeDecimal<Decimal128>::getScaleMultiplier(UInt32 scale_) {
    return common::exp10_i128(scale_);
}

/// Explicit template instantiations.
template class DataTypeDecimal<Decimal32>;
template class DataTypeDecimal<Decimal64>;
template class DataTypeDecimal<Decimal128>;

} // namespace doris::vectorized
