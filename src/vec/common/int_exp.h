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

#pragma once

#include <cstdint>
#include <limits>

#include "vec/core/defines.h"

/// On overlow, the function returns unspecified value.

inline NO_SANITIZE_UNDEFINED uint64_t intExp2(int x) {
    return 1ULL << x;
}

inline uint64_t intExp10(int x) {
    if (x < 0) return 0;
    if (x > 19) return std::numeric_limits<uint64_t>::max();

    static const uint64_t table[20] = {1ULL,
                                       10ULL,
                                       100ULL,
                                       1000ULL,
                                       10000ULL,
                                       100000ULL,
                                       1000000ULL,
                                       10000000ULL,
                                       100000000ULL,
                                       1000000000ULL,
                                       10000000000ULL,
                                       100000000000ULL,
                                       1000000000000ULL,
                                       10000000000000ULL,
                                       100000000000000ULL,
                                       1000000000000000ULL,
                                       10000000000000000ULL,
                                       100000000000000000ULL,
                                       1000000000000000000ULL,
                                       10000000000000000000ULL};

    return table[x];
}

namespace common {

inline int exp10_i32(int x) {
    static const int values[] = {1,      10,      100,      1000,      10000,
                                 100000, 1000000, 10000000, 100000000, 1000000000};
    return values[x];
}

inline int64_t exp10_i64(int x) {
    static const int64_t values[] = {1ll,
                                     10ll,
                                     100ll,
                                     1000ll,
                                     10000ll,
                                     100000ll,
                                     1000000ll,
                                     10000000ll,
                                     100000000ll,
                                     1000000000ll,
                                     10000000000ll,
                                     100000000000ll,
                                     1000000000000ll,
                                     10000000000000ll,
                                     100000000000000ll,
                                     1000000000000000ll,
                                     10000000000000000ll,
                                     100000000000000000ll,
                                     1000000000000000000ll};
    return values[x];
}

inline __int128 exp10_i128(int x) {
    static const __int128 values[] = {
            static_cast<__int128>(1ll),
            static_cast<__int128>(10ll),
            static_cast<__int128>(100ll),
            static_cast<__int128>(1000ll),
            static_cast<__int128>(10000ll),
            static_cast<__int128>(100000ll),
            static_cast<__int128>(1000000ll),
            static_cast<__int128>(10000000ll),
            static_cast<__int128>(100000000ll),
            static_cast<__int128>(1000000000ll),
            static_cast<__int128>(10000000000ll),
            static_cast<__int128>(100000000000ll),
            static_cast<__int128>(1000000000000ll),
            static_cast<__int128>(10000000000000ll),
            static_cast<__int128>(100000000000000ll),
            static_cast<__int128>(1000000000000000ll),
            static_cast<__int128>(10000000000000000ll),
            static_cast<__int128>(100000000000000000ll),
            static_cast<__int128>(1000000000000000000ll),
            static_cast<__int128>(1000000000000000000ll) * 10ll,
            static_cast<__int128>(1000000000000000000ll) * 100ll,
            static_cast<__int128>(1000000000000000000ll) * 1000ll,
            static_cast<__int128>(1000000000000000000ll) * 10000ll,
            static_cast<__int128>(1000000000000000000ll) * 100000ll,
            static_cast<__int128>(1000000000000000000ll) * 1000000ll,
            static_cast<__int128>(1000000000000000000ll) * 10000000ll,
            static_cast<__int128>(1000000000000000000ll) * 100000000ll,
            static_cast<__int128>(1000000000000000000ll) * 1000000000ll,
            static_cast<__int128>(1000000000000000000ll) * 10000000000ll,
            static_cast<__int128>(1000000000000000000ll) * 100000000000ll,
            static_cast<__int128>(1000000000000000000ll) * 1000000000000ll,
            static_cast<__int128>(1000000000000000000ll) * 10000000000000ll,
            static_cast<__int128>(1000000000000000000ll) * 100000000000000ll,
            static_cast<__int128>(1000000000000000000ll) * 1000000000000000ll,
            static_cast<__int128>(1000000000000000000ll) * 10000000000000000ll,
            static_cast<__int128>(1000000000000000000ll) * 100000000000000000ll,
            static_cast<__int128>(1000000000000000000ll) * 100000000000000000ll * 10ll,
            static_cast<__int128>(1000000000000000000ll) * 100000000000000000ll * 100ll,
            static_cast<__int128>(1000000000000000000ll) * 100000000000000000ll * 1000ll};
    return values[x];
}

} // namespace common
