#include "gtest/gtest.h"
#include "vec/columns/column_const.h"
#include "vec/columns/columns_number.h"

namespace doris::vectorized {
TEST(ColumnConstTest, BasicTest) {
    auto c32 = ColumnInt32::create();
    c32->insertDefault();
    auto const_v32 = ColumnConst::create(c32->getPtr(), 4096);
    auto cc = const_v32->convertToFullColumnIfConst();
    ASSERT_EQ(cc->size(), (size_t)4096);
    for (int i = 0; i < 4096; ++i) {
        ASSERT_EQ(0, (*cc)[i].get<Int32>());
    }
}
} // namespace DB

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
