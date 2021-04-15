#include "vec/columns/column_decimal.h"

#include "gtest/gtest.h"

namespace doris::vectorized {
TEST(ColumnDecimalTest, ColumnDecimalTest) {
   auto column_decimal_vector = ColumnDecimal<Decimal128>::create(4, 9);
   column_decimal_vector->insertDefault();

   __int128 f1 = 1;
   __int128 f2 = 2;
   __int128 f3 = 3;

   column_decimal_vector->insertData((const char*)&f1, sizeof(__int128));
   column_decimal_vector->insertData((const char*)&f2, sizeof(__int128));
   column_decimal_vector->insertData((const char*)&f3, sizeof(__int128));

    ASSERT_EQ(column_decimal_vector->size(), (size_t)8);
    ASSERT_EQ(0, (*column_decimal_vector)[0].get<Decimal128>());
    ASSERT_EQ(0, (*column_decimal_vector)[1].get<Decimal128>());
    ASSERT_EQ(0, (*column_decimal_vector)[2].get<Decimal128>());
    ASSERT_EQ(0, (*column_decimal_vector)[3].get<Decimal128>());
    ASSERT_EQ(0, (*column_decimal_vector)[4].get<Decimal128>());
    ASSERT_EQ(1, (*column_decimal_vector)[5].get<Decimal128>());
    ASSERT_EQ(2, (*column_decimal_vector)[6].get<Decimal128>());
    ASSERT_EQ(3, (*column_decimal_vector)[7].get<Decimal128>());
}
} // namespace DB

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

