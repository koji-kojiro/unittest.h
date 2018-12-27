# test.h
`test.h` is a simple unit-testing framework for C programmng language.  
This library is designed to work with gcc, assuming some specific attributes and nested functions are available.
`test.h` is meant to be a simple but better replacement of traditional `abort` tests.
The main features are listed below:

- header only.
- simply designed APIs.
- can handle segfault.
- colored reports.

The name of APIs and the output styles are a bit inspired with [Rove](github.com/fukamachi/rove), one of the unit-testing framework for Common Lisp.  
Note that this library is quite experimentally, so use at your own risks.

## APIs

### ok (expression)
Tests the evaluation results of expression, assuming the result true (non-zero).

### ng (expression)
Tests the evaluation results of expression, assuming the result false (zero).

### skip (expression)
Does nothing.

### unittest { ... }
Create a special block for defining test cases on the top level.

### setup, teardown
You can define these two functions for preparation and cleanup of tests.
`void setup (void)` is called before all the tests start, `void teardown (void)` is called after all the tests end.

### NDEBUG
If defined, all the APIs are disabled. 
You can pass `-DNDEBUG` flag to compiler to disable tests.

Additionally, note that `test.h` installs a signal handler for SIGSEGV. When segmentation fault occurs while running tests, process will be aborted immediately and test will fail. The signal handler basically does nothing, just tries to exit, but if the signal is reported inside tests, you can know in which test case the signal occures.

## Example
Here is a trivial example.

```:.c
#include "test.h"

int a = 10;

unittest
{
  ok (a == 10);
  ng (a == 0);
}

int
main (int argc, char *argv[])
{
  ok (a == 10);
  skip (a == 10);
  ng (a == 10);
  return 0;
}
```

You can get output like:

```:
running..... done:
  ✓ "a == 10" expected to be true.
  ✓ "a == 0" expected to be true.
  ✓ "a == 10" expected to be true.
  - "a == 10" skipped.
  ✗ "a == 10" expected to be false.
summary:
  ✗ 1 of 5 tests failed.
  - 1 tests skipped.
  ✓ 3 tests passed.
```

Each lines are colored according to its status.
The exit code will be overwritten according to the test results.  

Here is one more example:

```:.c
#include "test.h"

int
main (int argc, char *argv[])
{
  ok (1);               // <- passed
  char *c;
  ok ((c[0] = 2) == 2); // <- segmentation fault
  ok (1);               // <- not reached
  return 0;
}
```

The output will be:

```
running. aborted:
  ✓ "1" expected to be true.
  ✗ "(c[0] = 2) == 2" segmentation fault. aborted.
summary:
  ✗ 1 of 2 tests failed.
  ✓ 1 tests passed.
```

## License
test.h is distributed under [MIT License](LICENSE).

## Author
[TANI Kojiro](https://github.com/koji-kojiro) (kojiro0531@gmail.com) 
