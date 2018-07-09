/*
 * Copyright (c) 2017 Trail of Bits, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <deepstate/DeepState.hpp>
#include "functions.h"

using namespace deepstate;

DEEPSTATE_NOINLINE int add(int x, int y) {
  return x + y;
}

DEEPSTATE_NOINLINE int multiply(int x, int y) {
  return x * y;
}

TEST(Distributive, MultiplicationIsDistributive) {
  ForAll<int, int, int>([] (int x, int y, int z) {
    ASSERT_EQ(multiply(x, add(y, z)), add(multiply(x, y), multiply(x, z)))
        << "Multiplication of signed integers must distribute.";
  });
}

int main(int argc, char *argv[]) {
  DeepState_InitOptions(argc, argv);
  return DeepState_Run();
}
