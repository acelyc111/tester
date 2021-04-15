#include <iostream>
#include <memory>
#include <string>

#include "vec/aggregate_functions/aggregate_function_simple_factory.h"
#include "vec/aggregate_functions/aggregate_function.h"
#include "vec/columns/column_vector.h"
#include "vec/data_types/data_types_number.h"
#include "vec/data_types/data_type.h"
#include "gtest/gtest.h"

#include "vec/columns/column_nullable.h"
#include "vec/aggregate_functions/aggregate_function_null.h"

namespace doris::vectorized {
// declare function
void registerAggregateFunctionSum(AggregateFunctionSimpleFactory& factory);
void registerAggregateFunctionCombinatorNull(AggregateFunctionSimpleFactory & factory);

TEST(AggTest, basic_test) {
    auto column_vector_int32 = ColumnVector<Int32>::create();
    for (int i = 0; i < 4096; i++) {
        column_vector_int32->insert(castToNearestFieldType(i));
    }
    // test implement interface
    AggregateFunctionSimpleFactory factory;
    registerAggregateFunctionSum(factory);
    DataTypePtr data_type(std::make_shared<DataTypeInt32>());
    DataTypes data_types = {data_type};
    Array array;
    auto agg_function = factory.get("sum", data_types, array);
    AggregateDataPtr place = (char*)malloc(sizeof(uint64_t) * 4096);
    agg_function->create(place);
    const IColumn* column[1] = {column_vector_int32.get()};
    for (int i = 0; i < 4096; i++) {
        agg_function->add(place, column, i, nullptr);
    }
    int ans = 0;
    for (int i = 0; i < 4096; i++) {
        ans += i;
    }
    std::cout << ans << std::endl;
    std::cout << *(int32_t*)place << std::endl;
    ASSERT_EQ(ans, *(int32_t*)place);
    agg_function->destroy(place);
}

TEST(AggTest, nullable_test) {
    auto column_vector_int32 = ColumnVector<Int32>::create();
    auto column_nullable_vector = makeNullable(std::move(column_vector_int32));
    auto mutable_nullable_vector = std::move(*column_nullable_vector).mutate();
    for (int i = 0; i < 4096; i++) {
        mutable_nullable_vector->insert(castToNearestFieldType(i));
    }
    AggregateFunctionSimpleFactory &factory = AggregateFunctionSimpleFactory::instance();
    auto data_type = makeNullable(std::make_shared<DataTypeInt32>());
    std::string name = "sum";
    DataTypes data_types = {data_type};
    Array array;
    auto agg_function = factory.get(name, data_types, array);

    AggregateDataPtr place = (char*)malloc(sizeof(uint64_t) * 4096);
    agg_function->create(place);
    const IColumn* column[1] = {mutable_nullable_vector.get()};
    for (int i = 0; i < 4096; i++) {
        agg_function->add(place, column, i, nullptr);
    }
    uint64_t ans = 0;
    for (int i = 0; i < 4096; i++) {
        ans += i;
    }
    auto res_type = agg_function->getReturnType();
    auto res_column = res_type->createColumn();
    agg_function->insertResultInto(place ,*res_column.get());

    std::cout << ans << std::endl;
    std::cout << res_column->get64(0) << std::endl;
    ASSERT_EQ(ans, res_column->get64(0));
    agg_function->destroy(place);
    // factory.registerFunction()
}

} // namespace DB

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
