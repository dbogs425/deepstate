# DeepState

DeepState -- a symbolic execution framework for C and C++ makes using fuzzing to test rather straightforward. Instead of writing a custom analysis, developers can simply annotate their C and C++. DeepState provides developers with a "Google Test-esque" UI, making testing with DeepState a familiar and easy process. The goal of this article is to demonstrate how one can incorporate DeepState into a typical C/C++ application.

## Our project will have a simple architecture:
```
distribuive_test

    | -- deepstate

    | -- executable
```
* * * * *

Let's begin with cloning deepstate into the root directory (/distributive_test) with:
```
$ git clone https://github.com/trailofbits/deepstate deepstate
```
* * * * *

Some changes must be made to the CMakeLists.txt file within /deepstate in order for CMake to build properly:
```
Lines 61, 65, 70, 81: change ${CMAKE_SOURCE_DIR} to ${CMAKE_CURRENT_SOURCE_DIR}

Line 82: change ${CMAKE_CURRENT_BINARY_DIR} to ${CMAKE_BINARY_DIR}

Line 85: change ${CMAKE_BINARY_DIR} to ${CMAKE_CURRENT_BINARY_DIR}

Also, you must change setup.py.in within /deepstate/bin for the same reason:

Line 25: change to package_dir={"": "${CMAKE_CURRENT_SOURCE_DIR}/bin"}
```
* * * * *

Within our root directory, here is the CMakeLists.txt file I have created for this project:
```
cmake_minimum_required(VERSION 2.8)

project(distributive)

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

set(CMAKE_DISABLE_SOURCE_CHANGES ON)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_COLOR_MAKEFILE ON)

SET(CMAKE_CXX_FLAGS "-std=c++11 -O3")

#include and link deepstate lib

add_subdirectory(deepstate)

include_directories(${PROJECT_SOURCE_DIR}/deepstate/src/include)

link_directories(${PROJECT_SOURCE_DIR}/deepstate/src/lib)

#link our executable dir

add_subdirectory(executable)
```
* * * * *

Suppose you have a header file that exports two functions: add and multiply. You want to test these functions-- specifically that they always distribute. Namely, add(multiply(x, y), (multiply(x,z)) == multiply(x,add(y,z)).

For reference: the header file "functions.h" provides the following add/multiply functions and will reside in /executable :
```
int add(int x, int y) {

return x + y;

}

int multiply(int x, int y) {

return x * y;

}
```
* * * * *

Below is the implementation of DeepState to test the Distributive property. I will break down the basic components behind the testing.
```
#include <deepstate/DeepState.hpp>

#include "functions.h"

using namespace deepstate;

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
```
The TEST macro takes two parameters: a test case name and a test name, which follows Google Test conventions. We provide the test case name as "Distributive" and the test parameter as "MultiplicationIsDistributive."

We are symbolically testing all values that could be provided, so, we pass the ForAll statement 3 (signed) integers-- x, y, and z.

Then we write the following function to test the distributive property within our test:
```
ASSERT_EQ(multiply(x, add(y, z)), add(multiply(x, y), multiply(x, z)))
```
DeepState uses the same binary comparison methods as Google Test:

|      Fatal assertion      |    Nonfatal assertion     |    Verifies     |
|---------------------------|---------------------------|-----------------|
| ASSERT_EQ ( val1, val2 ); | EXPECT_EQ ( val1, val2 ); | val1 == val2    |
| ASSERT_NE ( val1, val2 ); | EXPECT_NE ( val1, val2 ); | val1 != val2    |
| ASSERT_LT ( val1, val2 ); | EXPECT_LT ( val1, val2 ); | val1 < val2  |
| ASSERT_LE ( val1, val2 ); | EXPECT_LE ( val1, val2 ); | val1 <= val2 |
| ASSERT_GT ( val1, val2 ); | EXPECT_GT ( val1, val2 ); | val1 > val2  |
| ASSERT_GE ( val1, val2 ); | EXPECT_GE ( val1, val2 ); | val1 >= val2 |



Then we add an annotation within the test to output what's being tested in case of a failed test case. Since tests are simply cpp streams, we use '<< ' followed by a string, to set the output to "Multiplication of signed integers must distribute," on a failed test case.

* * * * *

We must now create a CMakeLists.txt File Accompaniment within /executable:
```
cmake_minimum_required(VERSION 2.8)

project(Executable C CXX)

add_executable(Distributive Distributive.cpp)

find_library(DEEPSTATE_LIB deepstate deepstate32 ${CMAKE_SOURCE_DIR}/deepstate/src/include)

target_link_libraries(Distributive deepstate)
```
Now that we've covered the implementation of DeepState, let's see what happens when our test is executed.

* * * * *

## Executing with DeepState

To compile our application with CMake we must navigate to our root directory and execute the following commands:
```
$ mkdir build && cd build

$ cmake ../

$ make

To run DeepState, it's recommended to create a virtual environment for the python component. To do so, we must run the following commands:

$ virtualenv venv

$ . venv/bin/activate

$ python setup.py install
```
More on this can be seen within the Trail of Bits docs for DeepState although it does differ slightly due to difference in project architecture.

Now our tests are capable of being run. To do this, we can either run the program with manticore or angr.

We may run the test with the following CLI command from our root directory:
```
deepstate-angr --num_workers 4 --output_test_dir out deepstate/build/examples/Distribute
```
The output we receive will look like this:
```
(venv) root@ubuntu-s-1vcpu-1gb-nyc1-01:~/distributive_test# deepstate-angr --num_workers 4 --output_test_dir out deepstate/build/examples/Distributive

WARNING | 2018-04-17 05:41:30,099 | angr.analyses.disassembly_utils | Your verison of capstone does not support MIPS instruction groups.

INFO | 2018-04-17 05:41:36,804 | deepstate.angr | Running 1 tests across 4 workers

WARNING | 2018-04-17 05:41:42,605 | angr.manager | No completion state defined for SimulationManager; stepping until all states deadend

INFO | 2018-04-17 05:41:43,572 | deepstate | Running Distributive_MultiplicationIsDistributive from /root/deepstate/examples/Distributive.cpp(22)

INFO | 2018-04-17 05:41:43,572 | deepstate | Passed: Distributive_MultiplicationIsDistributive

INFO | 2018-04-17 05:41:43,572 | deepstate | Input: 00 00 00 00 00 00 00 00 00 00 00 00

INFO | 2018-04-17 05:41:43,573 | deepstate | Saving input to out/Distributive.cpp/Distributive_MultiplicationIsDistributive/8dd6bb7329a71449b0a1b292b5999164.pass
```
As you can see, our test has passed, and our functions indeed follow the distributive property!

Now, to provide an example of a failing test case I have changed the function that tests the distributive property to:
```
ASSERT_NE(multiply(x, add(y, z)), add(multiply(x, y), multiply(x, z)))
```
The output we receive will look like this:
```
(venv) root@ubuntu-s-1vcpu-1gb-nyc1-01:~# deepstate-angr --num_workers 4 --output_test_dir out deepstate/build/examples/Distributive
```
```
WARNING | 2018-04-17 06:01:26,634 | angr.analyses.disassembly_utils | Your verison of capstone does not support MIPS instruction groups.

INFO | 2018-04-17 06:01:33,194 | deepstate.angr | Running 1 tests across 4 workers

WARNING | 2018-04-17 06:01:38,935 | angr.manager | No completion state defined for SimulationManager; stepping until all states deadend

INFO | 2018-04-17 06:01:41,907 | deepstate | Running Distributive_MultiplicationIsDistributive from /root/deepstate/examples/Distributive.cpp(22)

CRITICAL | 2018-04-17 06:01:41,908 | deepstate | /root/deepstate/examples/Distributive.cpp(24): Multiplication of signed integers must distribute.

ERROR | 2018-04-17 06:01:41,908 | deepstate | Failed: Distributive_MultiplicationIsDistributive

INFO | 2018-04-17 06:01:41,908 | deepstate | Input: 00 00 00 00 00 00 00 00 00 00 00 00

INFO | 2018-04-17 06:01:41,908 | deepstate | Saving input to out/Distributive.cpp/Distributive_MultiplicationIsDistributive/8dd6bb7329a71449b0a1b292b5999164.fail
```
This is how simple it is to integrate and execute DeepState for testing.

## For more info on DeepState installation/background visit Trail of Bits' [Docs](https://github.com/trailofbits/deepstate) on Github and [paper](https://agroce.github.io/bar18.pdf).
