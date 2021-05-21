# CppND-System-Monitor

This is my implementation of the System Monitor Project in the Object Oriented Programming Course of the [Udacity C++ Nanodegree Program](https://www.udacity.com/course/c-plus-plus-nanodegree--nd213).

![System Monitor](images/monitor.png)

## Quickstart
Run the following command to build and run a docker container running the resulting executable of the code in this repository:
```bash
make launch
```

## Make
This project uses [Make](https://www.gnu.org/software/make/). The Makefile has five targets:
* `build` compiles the source code and generates an executable
* `format` applies [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) to style the source code
* `debug` compiles the source code and generates an executable, including debugging symbols
* `clean` deletes the `build/` directory, including all of the build artifacts
* `launch` builds and runs a docker container running the resulting executable of the code in this repository
