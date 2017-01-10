# eldispacho

## Introduction

eldispacho (Dispatcher) is a singleton that simulates the quantum network itself. Each quantum client connects to the dispatcher, which then simulates channels for communication. Eldispacho has a sense of time and space and thus is where the noise model is located.

## Installation

Eldispacho requires the following dependencies:
* CMake for build process management
* zmq for client communication
* cppzmq for C++ bindings to zmq
* RapidJSON for client RPC encoding/decoding
* (optional) Doxygen for code documentation

Ensure all required items are installed on your system before attempting to build. If you do not have binaries available, each is available on Github:
* [CMake](https://github.com/Kitware/CMake)
* [0MQ](https://github.com/zeromq/libzmq)
* [Doxygen](https://github.com/doxygen/doxygen)

The rest of the dependencies are implemented as [Git submodules](https://git-scm.com/docs/git-submodule):
* RapidJSON
* cppzmq

For each submodule, it will be necessary to clone the codebase into the eldispacho root directory. To do so, execute the follwing:
```shell
$ git submodule update --init
```

Thus, a typical initial installation would look something like:
```shell
$ git submodule update --init
$ mkdir build
$ cd build
$ cmake ../
$ make
$ sudo make install
```

A typical update install would look like:
```shell
$ git pull
$ git submodule foreach git pull origin master
$ make
$ sudo make install
```

Various build options are available when running *cmake ../*

* -DBUILD_RELEASE={ON/OFF}

	Specify if you wish to build for release. If *NO*, then debug is selected and no compiler optimizations are made while assert statements are not stripped from the resulting binary. If *YES*, then release is selected and maximum compiler optimizations are made while assert statements are stripped from the resulting binary. It is important to note that once you run _cmake -BUILD_TYPE ../_ in a directory, CMake will cache the value even if you change it later. The easiest way around this is having two build directories, one for debug and one for release.

	Unless you are testing changes to the code, you should just stick with the default value.

* -DUSE_STATIC={ON/OFF}

	Specify if you wish to use static libraries when building.

## Running

Configuration at startup via command line arguments is minimal as only a handful of variables need to be set. 

parameter | name | type | required | default
--- | --- | --- | --- | ---
--rp | *rx server endpoint* | string | yes | *none*
--rt | *rx server thread count* | Uint | no | 1
--tp | *tx server endpoint* | string | yes | *none*
--tt | *tx server thread count* | Uint | no | 1
--s | *sabot location* | string | yes | *none*
--st | *sabot client thread count* | Uint | no | 1
--l | *logger server endpoint* | server | no | *none*

A word of caution: If you wish to set *interface* to localhost, you __must__ use 127.0.0.1 as zmq will not correctly parse the former.

## Documentation

To generate code documentation, execute the following:
```shell
$ doxygen docs.conf
```
This will generate and save documentation in docs/ which may be viewed by pointing your web browser to docs/html/index.html.

## Programming

### Style
Variables use camelCase while other object use underscore_seperated_words. Enums, constants, and statics usually use uppercase underscore_seperated_words.

