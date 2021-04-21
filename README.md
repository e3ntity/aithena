# aithena
Framework for running and benchmarking game AIs such as AlphaZero, AlphaGo and more.

# Building
Building the project requires [boost](https://www.boost.org/) libraries.

```bash
aithena/ $ sudo apt-get install libboost-all-dev                  # Install dependencies
aithena/ $ mkdir build/ && cd build/
aithena/build/ $ cmake -DCMAKE_PREFIX_PATH=<path_to_libtorch> ..  # Generate build files
aithena/build/ $ make                                             # Build executable
aithena/build/ $ ./aithena-az                                     # Run program
```

Also for building tests, see [Test](#test).

# Test
Aithena uses [googletest](https://github.com/google/googletest) for testing.
To build the tests, the googletest github repository must be placed under extern/googletest.

```bash
aithena/ $ mkdir extern/ && cd extern/
aithena/extern/ $ git clone https://github.com/google/googletest
aithena/extern/ $ mkdir ../build/ && cd ../build/
aithena/build/ $ cmake -DCMAKE_PREFIX_PATH=<path_to_libtorch> ..
aithena/build/ $ ARGS=<gtest_args> make test
```
