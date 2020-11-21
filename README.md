# aithena
Framework for running and benchmarking game AIs such as AlphaZero, AlphaGo and more.

# Building
Building the project requires [boost](https://www.boost.org/) libraries.

```bash
$ sudo apt-get install libboost-all-dev # Install dependencies
$ make                                  # Build executable
$ build/aithena                         # Run program
```

Also for building tests, see [Test](#test).

# Test
Aithena uses [googletest](https://github.com/google/googletest) for testing.
To build the tests, a working installation of googletest is required.
