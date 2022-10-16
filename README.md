CSR

<br/>

# Introduction

Foo

<br/><br/>

# Build Instructions

Dependencies

* GLFW
* Epoxy

<br/>

Prepare Repo

```shell
$ git clone csr.git --recursive-submodules
```

<br/>

Generate Makefiles using CMake

```shell
$ cd csr/
$ cmake -DCMAKE_BUILD_TYPE=Release -B build
```

<br/>

Build and Install

```shell
$ cd build/
$ sudo make all install
```

<br/>

Run CSR

... optional ld.so.conf stuff ...


```shell
$ cd /opt/csr
$ LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/opt/csr/lib ./bin/csr_app
```

<br/><br/>

# Roadmap

- [x] v0.0.0
  - [x] Core Libs
  - [x] Platform Abstraction Layer
  - [x] Example: hello_config
  - [x] Example: hello_platform
- [ ] v0.0.1
  - [ ] Kernel Layer
  - [ ] Example: hello_kernel
- [ ] v0.0.2
  - [ ] CSR Hub Application
  - [ ] User Interface
  - [ ] Applet System

