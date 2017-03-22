# Flight Software Guidelines/Development Practices
  The goal of this document is to set standards for the development of all software that will fly on the VT USIP CubeSat.
Our main priority is to ensure that any software that flies be as robust and reliable as possible.
In support of this goal, we propose the following:
  - All code will pass a robust series of integration of unit tests before being committed to the master branch.
  - Matlab code must have 100% code coverage using MathWorks Profiler
  - Following the automated testing procedures, code must be reviewed by at least one other member of the team before being committed.
  - All code should be developed to the JPL Coding Standard for safety-critical C.

Work will be tracked via a Github Issues, where high level tickets (e.g. “integrate ADCS software”) will be broken up into easily accomplish subtasks (e.g. “add sun-sensor reading to RTOS”, “low-level magnetorquer drivers”, etc.).
Each subtask will go through the branch and code review process.

## Branching Strategy
The master branch should always contain “flight ready” code.
That is, code which has been reviewed by at least one team member other than the author and has sufficient test coverage (see Testing for more information).
All changes should be made via feature branches: short-lived branches which live only long enough for a small piece of work to be completed.

## C & Matlab Code Testing:
All flight code shall have at least 100% line coverage in unit tests.
100% branch coverage is not  necessary, but a high degree of branch coverage (~95%) is expected.
Any code which is too complex to achieve 100% branch coverage is probably in violation of the coding standard anyway.
In addition to unit tests, the system should be subject to a battery of integration tests, preferably running on real hardware, which ensure that the system as a whole is functioning as expected.
Matlab codes must have 100% profiler code coverage.
For C code static analysis will be performed using various static and dynamic profilers:
  - ASAN
  - TSAN
  - Valgrind

## Coding Style & Documentation:
  - All file/script name must be named meaningfully.
  - All initial code must have the author name, modification date/version number, general comments on the purpose of the code and documentation on all functions.
  - Define all variable, functions, methods with meaningful names. Magical numbers are not allowed.
  - One statement or declaration per line.
  - At least one blank line between function/methods
  - Indent all code appropriately
  - Every if, elseif/elif must have an else statement
  - Use parenthesis for formulas, boolean and liaison statements. 
  - All exception must be handled appropriately

Few key Notes From JPL: 
  - No direct or indirect recursion shall be used in any part of the code (Matlab and C)
  - Compile with all warnings enable (part of static clang-analyser for C)
  - Check loop boundary cases
  - No dynamic memory allocation after after task initialization
  - Check the return value of all non-void functions/system calls
  - Place no more than one statement or declaration per line
  - Use short function with few number of parameters

Be sure to read the rest of the [JPL coding standards](http://lars-lab.jpl.nasa.gov/JPL_Coding_Standard_C.pdf).

## Building
Build strategy depends on what you want to do and what platform you're building on.

### Test code, on Windows
In the root of the project, run
```
cmake -G 'Visual Studio'
```

### Test code, on Linux (CLI development)
In the root of the project, run
```
mkdir -p build-host
cd build-host
cmake ../
```
Running `make` in this directory will build the test binary

### MSP430 code, on Linux
Run the `build.sh` script.

### MSP430, on Windows
Not yet implemented.
You may be able to build things using commands similar to what `build.sh` does but this hasn't been testd.

# About
For more information email: vtcubesat@gmail.com

Written by VT USIP Flight Software Team.
