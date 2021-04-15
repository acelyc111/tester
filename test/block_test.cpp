#include "vec/core/block.h"
#include "vec/columns/column_vector.h"
#include "vec/data_types/data_types_number.h"

#include "vec/core/block_info.h"
#include "gtest/gtest.h"

namespace doris::vectorized {
TEST(BlockTest, BlockInfoTest) {
    BlockInfo info;
}

TEST(BlockTest, BlockMissingValuesTest) {
    BlockMissingValues bmv;
    bmv.setBit(1, 1);
    bmv.setBit(1, 2);
    BlockMissingValues::RowsBitMask mask = bmv.getDefaultsBitmask(1);
    ASSERT_EQ(mask[1], true);
    ASSERT_EQ(mask[2], true);
    ASSERT_EQ(mask[0], false);
}
TEST(BlockTest, BlockBasicTest) {
    auto vec = ColumnVector<Int32>::create();

    // reference to DataTypesNumber.cpp
    DataTypePtr data_type(std::make_shared<DataTypeInt32>());
    ColumnWithTypeAndName type_and_name(vec->getPtr(), data_type, "echo");
    Block block({type_and_name});
    ASSERT_EQ(block.getPositionByName("echo"), size_t(0));
}
} // namespace DB

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
