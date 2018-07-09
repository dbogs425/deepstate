**DeepState**

DeepState -- an symbolic execution framework for C and C++ makes using fuzzing to test rather straightforward. Instead of writing a custom analysis, developers can simply annotate their C and C++.  DeepState provides developers with a &quot;Google Test-esque&quot; UI, making testing with DeepState a familiar and easy process. The goal of this article is to demonstrate how one can incorporate DeepState into a typical C/C++ application.

Our project will have a simple architecture:

distribuive\_test

|   -- deepstate

|   -- executable

\* \* \* \* \*

Let&#39;s begin with cloning deepstate into the root directory (/distributive\_test) with:

$ git clone https://github.com/trailofbits/deepstate deepstate

\* \* \* \* \*

Some changes must be made to the CMakeLists.txt file within /deepstate in order for CMake to build properly:

Lines 61, 65, 70, 81: change ${CMAKE\_SOURCE\_DIR} to ${CMAKE\_CURRENT\_SOURCE\_DIR}

Line 82: change ${CMAKE\_CURRENT\_BINARY\_DIR} to ${CMAKE\_BINARY\_DIR}

Line 85: change ${CMAKE\_BINARY\_DIR} to ${CMAKE\_CURRENT\_BINARY\_DIR}

Also, you must change setup.py.in within /deepstate/bin for the same reason:

Line 25: change to package\_dir={&quot;&quot;: &quot;${CMAKE\_CURRENT\_SOURCE\_DIR}/bin&quot;}

\* \* \* \* \*

Within our root directory, here is the CMakeLists.txt file I have created for this project:

cmake\_minimum\_required(VERSION 2.8)

project(distributive)

set(CMAKE\_DISABLE\_IN\_SOURCE\_BUILD ON)

set(CMAKE\_DISABLE\_SOURCE\_CHANGES  ON)

set(CMAKE\_VERBOSE\_MAKEFILE ON)

set(CMAKE\_COLOR\_MAKEFILE   ON)

SET(CMAKE\_CXX\_FLAGS &quot;-std=c++11 -O3&quot;)

#include and link deepstate lib

add\_subdirectory(deepstate)

include\_directories(${PROJECT\_SOURCE\_DIR}/deepstate/src/include)

link\_directories(${PROJECT\_SOURCE\_DIR}/deepstate/src/lib)

#link our executable dir

add\_subdirectory(executable)

\* \* \* \* \*

Suppose you have a header file that exports two functions: add and multiply.  You want to test these functions-- specifically that they always distribute. More specifically, add(multiply(x, y), (multiply(x,z)) == multiply(x,add(y,z)).

For reference: the header file &quot;functions.h&quot; provides the following add/multiply functions and will reside in /executable :

int add(int x, int y) {

  return x + y;

}

int multiply(int x, int y) {

  return x \* y;

}

\* \* \* \* \*

Below is the implementation of DeepState to test the Distributive property.  I will break down the basic components behind the testing.

#include &lt;deepstate/DeepState.hpp&gt;

#include &quot;functions.h&quot;

using namespace deepstate;

TEST(Distributive, MultiplicationIsDistributive) {

 ForAll&lt;int, int, int&gt;([] (int x, int y, int z) {

   ASSERT\_EQ(multiply(x, add(y, z)), add(multiply(x, y), multiply(x, z)))

       &lt;&lt; &quot;Multiplication of signed integers must distribute.&quot;;

 });

}

int main(int argc, char \*argv[]) {

 DeepState\_InitOptions(argc, argv);

 return DeepState\_Run();

}

The TEST macro takes two parameters: a test case name and a test name, which follows [Google Test conventions](https://github.com/google/googletest/blob/master/googletest/docs/Primer.md#simple-tests).  We provide the test case name as &quot;Distributive&quot; and the test parameter as &quot;MultiplicationIsDistributive.&quot;

We are symbolically testing  all  values that could be provided, so, we pass the ForAll statement 3 (signed) integers-- x, y, and z.

Then we write the following function to test the distributive property within our test:

ASSERT\_EQ(multiply(x, add(y, z)), add(multiply(x, y), multiply(x, z)))

DeepState uses the same binary comparison methods as Google Test:

.---------------------------.---------------------------.-------------------.
|                           |                           |                   |  
|     Fatal assertion       |     Nonfatal assertion    |     Verifies      |
|                           |                           |                   |
:---------------------------+---------------------------+-------------------:
|                           |                           |                   |
| ASSERT\_EQ ( val1, val2 );| EXPECT\_EQ ( val1, val2 );|   val1 == val2    |
|                           |                           |                   |
:---------------------------+---------------------------+-------------------:
|                           |                           |                   |
| ASSERT\_NE ( val1, val2 );| EXPECT\_NE ( val1, val2 );|   val1 != val2    |
|                           |                           |                   |
:---------------------------+---------------------------+-------------------:
|                           |                           |                   |
| ASSERT\_LT ( val1, val2 );| EXPECT\_LT ( val1, val2 );|  val1 &lt; val2   |
|                           |                           |                   |
:---------------------------+---------------------------+-------------------:
|                           |                           |                   |
| ASSERT\_LE ( val1, val2 );| EXPECT\_LE ( val1, val2 );|  val1 &lt;= val2  |
|                           |                           |                   |
:---------------------------+---------------------------+-------------------:
|                           |                           |                   |
| ASSERT\_GT ( val1, val2 );| EXPECT\_GT ( val1, val2 );|  val1 &gt; val2   |
|                           |                           |                   |
:---------------------------+---------------------------+-------------------:
|                           |                           |                   |
| ASSERT\_GE ( val1, val2 );| EXPECT\_GE ( val1, val2 );|  val1 &gt;= val2  |
|                           |                           |                   |
&#39;---------------------------&#39;---------------------------&#39;--------------&#39;

Then we add an annotation within the test to output what&#39;s being tested in case of a failed test case.  Since tests are simply cpp streams, we use &#39;&lt;&lt; &#39; followed by a string, to set the output to &quot;Multiplication of signed integers must distribute,&quot; on a failed test case.

\* \* \* \* \*

We must now create a CMakeLists.txt File Accompaniment Within /executable:

cmake\_minimum\_required(VERSION 2.8)

project(Executable C CXX)

add\_executable(Distributive Distributive.cpp)

find\_library(DEEPSTATE\_LIB deepstate deepstate32 ${CMAKE\_SOURCE\_DIR}/deepstate/src/include)

target\_link\_libraries(Distributive deepstate)

Now that we&#39;ve covered the implementation of DeepState, let&#39;s see what happens when our test is executed.

\* \* \* \* \*

Executing with DeepState

To compile our application with CMake we must navigate to our root directory and execute the following commands:

$ mkdir build &amp;&amp; cd build

$ cmake ../

$ make

To run DeepState, it&#39;s recommended to create a virtual environment for the python component. To do so, we must run the following commands:

$ virtualenv venv

$ . venv/bin/activate

$ python setup.py install

More on this can be seen within the [Trail of Bits docs for DeepState](https://github.com/trailofbits/deepstate) although it does differ slightly due to difference in project architecture.

Now our tests are capable of being run.  To do this, we can either run the program with manticore or angr.

We may run the test with the following CLI command from our root directory:

deepstate-angr --num\_workers 4 --output\_test\_dir out deepstate/build/examples/Distribute

The output we receive will look like this:

(venv) root@ubuntu-s-1vcpu-1gb-nyc1-01:~/distributive\_test# deepstate-angr --num\_workers 4 --output\_test\_dir out deepstate/build/examples/Distributive

WARNING | 2018-04-17 05:41:30,099 | angr.analyses.disassembly\_utils | Your verison of capstone does not support MIPS instruction groups.

INFO    | 2018-04-17 05:41:36,804 | deepstate.angr | Running 1 tests across 4 workers

WARNING | 2018-04-17 05:41:42,605 | angr.manager | No completion state defined for SimulationManager; stepping until all states deadend

INFO    | 2018-04-17 05:41:43,572 | deepstate | Running Distributive\_MultiplicationIsDistributive from /root/deepstate/examples/Distributive.cpp(22)

INFO    | 2018-04-17 05:41:43,572 | deepstate | Passed: Distributive\_MultiplicationIsDistributive

INFO    | 2018-04-17 05:41:43,572 | deepstate | Input: 00 00 00 00 00 00 00 00 00 00 00 00

INFO    | 2018-04-17 05:41:43,573 | deepstate | Saving input to out/Distributive.cpp/Distributive\_MultiplicationIsDistributive/8dd6bb7329a71449b0a1b292b5999164.pass

As you can see, our test has passed, and our functions do indeed follow the distributive property!

Now, to provide an example of a failing test case I have changed the function that tests the distributive property to:

ASSERT\_NE(multiply(x, add(y, z)), add(multiply(x, y), multiply(x, z)))

The output we receive will look like this:

(venv) root@ubuntu-s-1vcpu-1gb-nyc1-01:~# deepstate-angr --num\_workers 4 --output\_test\_dir out deepstate/build/examples/Distributive

WARNING | 2018-04-17 06:01:26,634 | angr.analyses.disassembly\_utils | Your verison of capstone does not support MIPS instruction groups.

INFO    | 2018-04-17 06:01:33,194 | deepstate.angr | Running 1 tests across 4 workers

WARNING | 2018-04-17 06:01:38,935 | angr.manager | No completion state defined for SimulationManager; stepping until all states deadend

INFO    | 2018-04-17 06:01:41,907 | deepstate | Running Distributive\_MultiplicationIsDistributive from /root/deepstate/examples/Distributive.cpp(22)

CRITICAL | 2018-04-17 06:01:41,908 | deepstate | /root/deepstate/examples/Distributive.cpp(24): Multiplication of signed integers must distribute.

ERROR   | 2018-04-17 06:01:41,908 | deepstate | Failed: Distributive\_MultiplicationIsDistributive

INFO    | 2018-04-17 06:01:41,908 | deepstate | Input: 00 00 00 00 00 00 00 00 00 00 00 00

INFO    | 2018-04-17 06:01:41,908 | deepstate | Saving input to out/Distributive.cpp/Distributive\_MultiplicationIsDistributive/8dd6bb7329a71449b0a1b292b5999164.fail

This is how simple it is to integrate and execute DeepState for testing.

For more info on DeepState installation/background visit Trail of Bits&#39; [Docs on Github](https://github.com/trailofbits/deepstate) and their [paper](https://www.cefns.nau.edu/~adg326/bar18.pdf).
