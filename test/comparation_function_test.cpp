#include <iostream>
#include <memory>
#include <string>

#include "vec/columns/column_vector.h"
#include "vec/functions/functions_comparison.h"
#include "gtest/gtest.h"


namespace doris::vectorized {

TEST(ComparationTest, basic_test) {
    auto [column_vector_int32_1,column_vector_int32_2] = std::pair{ColumnVector<Int32>::create(), ColumnVector<Int32>::create()};
    for (int i = 0; i < 10; i++) {
        column_vector_int32_1->insert(castToNearestFieldType(i));
        column_vector_int32_2->insert(castToNearestFieldType(10 - i));
    }

    auto col_res = ColumnUInt8::create();
    ColumnUInt8::Container& vec_res = col_res->getData();
    vec_res.resize(10);
    NumComparisonImpl<Int32, Int32, GreaterOp<Int32, Int32>>::vector_vector(column_vector_int32_1->getData(),
           column_vector_int32_2->getData(), vec_res);

    for (int i = 0; i < 10; i++) {
        ASSERT_EQ(i > 10 - i, col_res->getBool(i));
    }
}

TEST(ComparationTest, different_test) {
    auto [column_vector_int32_1,column_vector_float64_2] = std::pair{ColumnVector<Int32>::create(), ColumnVector<Float64>::create()};
    for (int i = 0; i < 10; i++) {
        column_vector_int32_1->insert(castToNearestFieldType(i));
        column_vector_float64_2->insert(castToNearestFieldType(double(10 - i)));
    }

     auto col_res = ColumnUInt8::create();
     ColumnUInt8::Container & vec_res = col_res->getData();
     vec_res.resize(10);
     NumComparisonImpl<Int32, Float64, GreaterOp<Int32, Float64>>::vector_vector(column_vector_int32_1->getData(),
            column_vector_float64_2->getData(), vec_res);

     for (int i = 0; i < 10; i++) {
         ASSERT_EQ(i > 10 - i, col_res->getBool(i));
     }
}

} // namespace DB

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
