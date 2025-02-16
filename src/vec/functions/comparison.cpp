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

#include "vec/functions/functions_comparison.h"
#include "vec/functions/simple_function_factory.h"

namespace doris::vectorized {
using FunctionGreater = FunctionComparison<GreaterOp, NameGreater>;
using FunctionGreaterOrEquals = FunctionComparison<GreaterOrEqualsOp, NameGreaterOrEquals>;
using FunctionLess = FunctionComparison<LessOp, NameLess>;
using FunctionLessOrEquals = FunctionComparison<LessOrEqualsOp, NameLessOrEquals>;
using FunctionEquals = FunctionComparison<EqualsOp, NameEquals>;
using FunctionNotEquals = FunctionComparison<NotEqualsOp, NameNotEquals>;

void registerFunctionComparison(SimpleFunctionFactory& factory) {
    factory.registerFunction<FunctionGreater>();
    factory.registerFunction<FunctionGreaterOrEquals>();
    factory.registerFunction<FunctionLess>();
    factory.registerFunction<FunctionLessOrEquals>();
    factory.registerFunction<FunctionEquals>();
    factory.registerFunction<FunctionNotEquals>();
}
} // namespace doris::vectorized
