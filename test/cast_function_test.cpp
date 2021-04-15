#include "gtest/gtest.h"
#include "vec/core/block.h"
#include "vec/functions/function_cast.h"

namespace doris::vectorized {
TEST(FunctionCastTest, NumberInnerCast) {
    auto vec = ColumnVector<Int32>::create();
    auto& data = vec->getData();
    for (int i = 0; i < 1024; ++i) {
        data.push_back(i);
    }
    DataTypePtr data_type(std::make_shared<DataTypeInt32>());
    ColumnWithTypeAndName type_and_name(vec->getPtr(), data_type, "From");

    DataTypePtr data_type_2(std::make_shared<DataTypeInt64>());
    ColumnWithTypeAndName type_and_name_2(nullptr, data_type_2, "To");

    ColumnNumbers arguments(1);
    arguments[0] = 0;

    Block block({type_and_name, type_and_name_2});
    ConvertImpl<DataTypeInt32, DataTypeInt64, NameCast>::execute(block, arguments, 1, -1);
    int size = block.getByPosition(1).column->size();
    ASSERT_EQ(size, 1024);
    auto& data_2 = ((ColumnVector<Int64>*)block.getByPosition(1).column.get())->getData();

    for (int i = 0; i < 1024; ++i) {
        ASSERT_EQ(data_2[i], data[i]);
    }
}

TEST(FunctionCastTest, DecimalNumberCast) {
    // TEST Int32 to Decimal
    {
        auto veci32 = ColumnVector<Int32>::create();
        auto& data = veci32->getData();
        for (int i = 0; i < 1024; ++i) {
            data.push_back(i);
        }
        // auto vecdecimal = ColumnDecimal<Int128>::create(1024,9);
        DataTypePtr data_type_1(std::make_shared<DataTypeInt32>());
        ColumnWithTypeAndName type_and_name_1(veci32->getPtr(), data_type_1, "From");
        DataTypePtr data_type_2(createDecimal(27, 9));
        ColumnWithTypeAndName type_and_name_2(nullptr, data_type_2, "To");

        auto param = ColumnVector<UInt32>::create();
        DataTypePtr argument_type(std::make_shared<DataTypeUInt32>());
        param->insertValue(9); // scale == 9
        ColumnWithTypeAndName argument_type_and_name(param->getPtr(), argument_type, "From");

        Block block({type_and_name_1, type_and_name_2, argument_type_and_name});
        ColumnNumbers arguments(2); // here we need to arguments
        arguments[0] = 0;
        arguments[1] = 2;
        const ColumnWithTypeAndName& scale_column = block.getByPosition(arguments[1]);
        UInt32 scale = extractToDecimalScale(scale_column);
        ConvertImpl<DataTypeInt32, DataTypeDecimal<Decimal128>, NameCast>::execute(block, arguments,
                                                                                   1, -1, scale);
        auto column = block.getByPosition(1).column.get();
        ASSERT_EQ((int)column->size(), 1024);

        for (int i = 0; i < 1024; ++i) {
            auto v = (*column)[i].get<DecimalField<Decimal128>>();
            ASSERT_FLOAT_EQ(((double)v.getValue()) / (std::pow(10, v.getScale())), data[i]);
        }
    }
    // TEST Cast Decimal To Int32
    {
        DataTypePtr decimal_data_type(createDecimal(27, 9));
        auto decimal_column = decimal_data_type->createColumn();
        auto& data = ((ColumnDecimal<Decimal<Int128>>*)decimal_column.get())->getData();
        for (int i = 0; i < 1024; ++i) {
            __int128_t value = i;
            for (int j = 0; j < 9; ++j) {
                value *= 10;
            }
            data.push_back(value);
        }
        ColumnWithTypeAndName decimal_column_ctan(decimal_column->getPtr(), decimal_data_type,
                                                  "From");

        DataTypePtr data_type_i32(std::make_shared<DataTypeInt32>());
        ColumnWithTypeAndName i32_ctan(nullptr, data_type_i32, "From");

        Block block({decimal_column_ctan, i32_ctan});
        ColumnNumbers arguments(1); // here we need to arguments
        arguments[0] = 0;
        ConvertImpl<DataTypeDecimal<Decimal128>, DataTypeInt32, NameCast>::execute(block, arguments,
                                                                                   1, -1);

        auto column = block.getByPosition(1).column.get();
        for (int i = 0; i < 1024; ++i) {
            auto v = column->getInt(i);
            __int128_t value = data[i];
            for (int j = 0; j < 9; ++j) {
                value /= 10;
            }
            ASSERT_FLOAT_EQ(v, value);
        }
    }
}

TEST(FunctionCastTest, BasicCastString) {
    // cast int to string
    {
        auto veci32 = ColumnVector<Int32>::create();
        auto& data = veci32->getData();
        for (int i = 0; i < 1024; ++i) {
            data.push_back(i);
        }
        DataTypePtr data_type_1(std::make_shared<DataTypeInt32>());
        ColumnWithTypeAndName type_and_name_1(veci32->getPtr(), data_type_1, "From");

        DataTypePtr data_type_2(std::make_shared<DataTypeString>());
        ColumnWithTypeAndName type_and_name_2(nullptr, data_type_2, "To");

        Block block({type_and_name_1, type_and_name_2});
        ColumnNumbers arguments(1); // here we need to arguments
        arguments[0] = 0;

        int result = 1;
        ConvertImplGenericToString::execute(block, arguments, result);
        auto column = block.getByPosition(result).column;
        for (int i = 0; i < 1024; ++i) {
            auto string_ref = ((ColumnString*)column.get())->getDataAt(i);
            ASSERT_EQ(std::string(string_ref.data, string_ref.size), std::to_string(i));
        }
    }
    // cast int128
    // case decimal to string
}

TEST(FunctionCastTest, FunctionCastBuilderTest) {
    auto cast_builder = FunctionBuilderCast::create();
    {
        auto veci32 = ColumnVector<Int32>::create();
        auto& data = veci32->getData();
        for (int i = 0; i < 1024; ++i) {
            data.push_back(i);
        }
        DataTypePtr data_type_1(std::make_shared<DataTypeInt32>());
        ColumnWithTypeAndName type_and_name_1(veci32->getPtr(), data_type_1, "From");

        auto strcol = ColumnString::create();
        std::string target = "Int64";
        strcol->insertData(target.c_str(), target.size());
        auto const_param_col = ColumnConst::create(strcol->getPtr(), 1);

        DataTypePtr data_type_2(std::make_shared<DataTypeString>());

        // DataTypePtr data_type_1(std::make_shared<DataTypeInt64>());
        ColumnWithTypeAndName type_and_name_2(const_param_col->getPtr(), data_type_2,
                                              "params: cast to Int64");

        ColumnsWithTypeAndName arguments = {type_and_name_1, type_and_name_2};
        Block block(arguments);

        auto func = cast_builder->build(arguments);

        auto ret_type = func->getReturnType();
        block.insert({nullptr, ret_type, "cast result"});

        ColumnNumbers exec_arguments{0, 1};
        func->execute(block, exec_arguments, 2, block.rows(), false);

        auto& data_2 = ((ColumnVector<Int64>*)block.getByPosition(2).column.get())->getData();

        for (int i = 0; i < 1024; ++i) {
            ASSERT_EQ(data_2[i], data[i]);
        }
    }
}

} // namespace DB

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
