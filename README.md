# CSR

![CSR Banner](files/csr_feisar_frame.png)

## Overview

- [Introduction](#introduction)  
- [Build and Run](#build-and-run)  
- [Roadmap](#roadmap)  
- [Media](#media)  

## Introduction

> *“It’s still magic even if you know how it’s done.”*
>
> *― Terry Pratchett*

This Quote by Terry Pratchett is on point on how I feel about Computer Graphics. I always loved being immersed in nicely lit Environments, feeling the dense Atmosphere in unreal Places or just wondering how all of this was actually made. Back then it was Magic to me, just as it is today.

So welcome to my passion Project on which I work(ed) in my freetime, which is also the Playground for my CG related Research.

## Build and Run

Linux only for now.

**External Dependencies**

* CMake
* GLFW
* Epoxy

<br/>

Modern Features like DSA (Direct State Access) are used so at least OpenGL v4.5 is required.

**Checkout Repo and Submodules**

```shell
$ git clone https://github.com/papa-agoo/csr.git
$ cd csr/
$ git submodule update --init --recursive
```

<br/>

If you are swithing to a different Branch or simply fetching new Commits, you should always sync the internal Dependencies (like ImGui) using `git submodule update`.

```shell
$ git checkout origin/develop -b develop
$ git submodule update --recursive
```

<br/>

**Generate Makefiles using CMake**

```shell
$ cmake -DCMAKE_BUILD_TYPE=Release -B build
```

<br/>

**Build and Install**

```shell
$ cd build/
$ sudo make all install
```

<br/>

**Run CSR**

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

## Roadmap

The main Focus lies on the Model Viewer Applet (model_viewer.so) which will be able to load and render (on GPU and on CPU) external Models in Wavefront OBJ / glTF Formats. Stuff like Bug Fixes, Refactoring or smaller Detours (which will happen :)) are not listed here.

<br/>

* **Milestone 1 (Prep Work)**
  - [ ] Application
    - [ ] Env Vars Service (Support for something like {FOO}/{BAR}.baz at Runtime)
    - [ ] Per Applet Config ({APPLET_CONFIG_DIR}/my_applet.ini)
    - [ ] App Home Dir ({USER_HOME_DIR}/.csr)
  - [ ] Library
    - [ ] Modern String Impl. (length based)
    - [ ] Arena Memory Allocator (Heap und Pool Allocators coming later)
    - [ ] File I/O (OS level R/W API)

<br/>

* **Milestone 2 (Foundations)**
  - [ ] Application
    - [ ] Reload current Applet
    - [ ] Remember last loaded Applet
  - [ ] Library
    - [ ] SoftGL Rasterizer
  - [ ] Applets
    - [ ] ecs_hello_world.so (some Tests with Flecs)
    - [ ] xgl_hello_cube.so (textured)
    - [ ] model_viewer.so
      - [ ] 3D Scene (Base Grid, Orbit Camera, Orientantion Gizmo, Unit Cube AABB)
      - [ ] User Interface (Model, Camera, Renderers, Debug Info)
      - [ ] GPU + CPU Renderers (XGL + SoftGL)

<br/>

* **Milestone 3 (Mesh Loading + Debug Materials)**
  - [ ] Library
    - [ ] SoftGL Texture Mapping
  - [ ] model_viewer.so
    - [ ] Builtin procedural Meshes (Cube, Sphere, Torus, ...)
    - [ ] Loaders for OBJ / GLTF Files
    - [ ] Debug Materials (Points, Lines, Normals, UVs, Solid, Textured)
    - [ ] Integration of the ImGizmo Library

<br/>

* **Milestone 4 (Lighting + Environment)**
  - [ ] model_viewer.so
    - [ ] Legacy Lighting
    - [ ] Soft Shadows
    - [ ] Environment (Sky, Fog, ...)

<br/>

* **Milestone X**
  - [ ] Rendering
    - [ ] PBR Pipeline
    - [ ] Post Processing
    - [ ] XGL SDF Text
  - ...

## Media

Very early Version. This little Project ignited my Passion for Graphics Programming a few years ago. :)

![CSR Legacy](files/csr_legacy.png)
