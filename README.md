# Tinydbg

## Setup

### Install Dependencies for Ubuntu

```
$ sudo apt-get install -y build-essential clang llvm-dev lldb libncurses5-dev python3 pkg-config
```

### Install libelfin

First of all, compile and install libelfin as follows.
```
$ cd ($Tinydbg)/ext/libelfin
$ make && make install
```

## Compilation and Execution

Compile tinydbg and a hello world application,
```
$ cd ($Tinydbg)/src/
$ make
```

Run the hello world application.
```
$ ./hello
hello
world
```

Run the hello world application on tinydbg
```
$ ./tinydbg ./hello
tinydbg> continue
hello
world
tinydbg>
```