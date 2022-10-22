# CSR

... introduction ... Ponyhof ... [Andreas Gummer](https://github.com/papa-agoo) ...

<br/>

## Build and Run

... linux only ...

<br/>

Dependencies

* GLFW
* Epoxy

<br/>

Prepare Repo

```shell
$ git clone https://github.com/papa-agoo/csr.git --recursive-submodules
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

... assuming `/opt/csr` is the install prefix (project root directory) set the `LD_LIBRARY_PATH` before executing CSR apps ...

```shell
$ export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/opt/csr/lib
```

... or just add `/opt/csr/lib` to `ld.so.conf` once and forget about it ...

<br/>

... for now CSR apps must be run from within the `project root directory` (i.e. /opt/csr) ...

```shell
$ cd /opt/csr
$ ./bin/csr_app
```

<br/><br/>

## Roadmap

... milestones dings ...

<br/>

- [ ] Milestone 1 : Foundation
  - [x] Core Libs Layer
  - [x] Platform Abstraction Layer
  - [ ] Kernel Layer
  - [ ] Hub Application
    - [ ] Config Files
    - [ ] User Interface
    - [ ] Applet System
- [ ] Milestone 2 : Simple Applets
  - [ ] xgl_hello_triangle
  - [ ] hello_screens
  - [ ] softgl_hello_triangle
  - [ ] ...
- [ ] Milestone 3 : Rendering System Foundation
  - [ ] ...
  - [ ] applet: rsx_hello_world
  - [ ] ...

<br/><br/>

... milestone candidates ...

<br/>

- [ ] Milestone ? : Resource System
  - [ ] ...
  - [ ] applet: hello_resource
  - [ ] ...

<br/><br/>

## Media

<br/>

Very early Version.
![CSR Legacy](files/csr_legacy.png)
