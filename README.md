# CSR

... introduction ... Ponyhof ... [Andreas Gummer](https://github.com/papa-agoo) ...

<br/>

## Build and Run

Currently only Linux is supported but it should be easy to do a Port for Windows though.

<br/>

Dependencies

* CMake
* GLFW
* Epoxy

<br/>

Checkout Repo and Submodules

```shell
$ git clone https://github.com/papa-agoo/csr.git
$ git submodules update --init --recursive
$ cd csr/
```

<br/>

If you are swithing to a different Branch or simply fetching new Commits, you should always sync the internal Dependencies (like ImGui) using `git submodule update`.

```shell
$ git checkout origin/develop -b develop
$ git submodule update --recursive
```

<br/>

Generate Makefiles using CMake

```shell
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

Assuming `/opt/csr` is the install prefix (project root directory) set the `LD_LIBRARY_PATH` before executing CSR Apps ...

```shell
$ export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/opt/csr/lib
```

... or just add `/opt/csr/lib` to `ld.so.conf` once and forget about it.

<br/>

For now CSR Apps must be run from within the `project root directory`.

```shell
$ cd /opt/csr
$ ./bin/csr_app
```

<br/><br/>

## Roadmap

... priority table ...

<br/><br/>

## Media

<br/>

Very early Version.
![CSR Legacy](files/csr_legacy.png)
