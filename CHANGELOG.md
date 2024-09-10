# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [0.0.4] - 2024-09-10
### Added
- Application
  - Env Vars (support for something like {FOO}/{BAR}.baz at runtime)
  - Per applet config ({APPLET_CONFIG_DIR}/my_applet.ini)
  - App home dir ({USER_HOME_DIR}/.csr)
- Library
  - Modern string impl. (immutable, length based)
  - Arena memory allocator (bare bones)
  - File I/O (OS level R/W API)

## [0.0.3] - 2022-12-06
### Added
- Application
  - Frontend
  - Persistent Configs (application.ini, imgui.ini)
  - Applet Loading
- User Interface
  - Workspaces, Contexts, Docking
  - Menus, Windows, Views, Properties
  - Theme Support, Custom Fonts, DPI Awareness (sort of)
  - Basic Window Management
- Applet System
  - Applet DB
  - Applet Manager
  - Applet (Plugin)
  - Applet I/O Layer (aio)
- Applets
  - minimal.so
  - hello_world.so

## [0.0.2] - 2022-10-23
### Added
- XGL (Vulkan like API for modern OpenGL)
- Kernel Layer
  - Logging
  - Event System
  - Services (Mainloop, Video, ...)
- Example Apps
  - hello_kernel

## [0.0.1] - 2022-10-16
### Added
- Core Libs Layer
- Platform Abstraction Layer
- Example Apps
  - hello_config
  - hello_platform

## [0.0.0] - 2022-10-10
- Big Bang ;)

[0.0.4]: https://github.com/papa-agoo/csr/compare/v0.0.3...v0.0.4
[0.0.3]: https://github.com/papa-agoo/csr/compare/v0.0.2...v0.0.3
[0.0.2]: https://github.com/papa-agoo/csr/compare/v0.0.1...v0.0.2
[0.0.1]: https://github.com/papa-agoo/csr/compare/v0.0.0...v0.0.1
[0.0.0]: https://github.com/papa-agoo/csr/compare/v0.0.0...v0.0.0
