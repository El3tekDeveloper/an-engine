# Project Architecture

The project is divided into modules.

Each module has a specific responsibility and communicates with other modules through clearly defined boundaries.

Each module contains its own Meson build file and is added to the main Meson build using `subdir()`.

---

## Core

**Rules**

- Independent from the rest of the engine.
- Must not depend on any other engine module.
- Only provides functionality to other modules.
- Breaking this rule is allowed only in very rare cases.

#### Debug

Logging, assertions, and debugging utilities.

#### Input

Keyboard, mouse, and other input devices.

#### IO

File and data input/output.

#### Math

Math types and functions.

#### Memory

Memory allocators and utilities.

#### Object

Base object system and object utilities.

#### OS

Operating system abstraction.

#### Utils

General utilities used across the engine.

---

## Main

**Rules**

- Entry point of the engine.
- Contains only the main loop and startup/shutdown logic.
- Does not expose functionality to other modules.
- Should not be used as a dependency.

The main application loop.

---

## Render

**Rules**

- Mostly independent.
- Can depend only on **Core** and **Resources**.
- Should not communicate directly with gameplay or scene logic.

Rendering backend interfaces.

#### OpenGL

The OpenGL rendering backend.

#### Shaders

The shaders used by the rendering backend.

---

## Resources

**Rules**

- Shared module used by multiple engine systems.
- Contains shared resource data and resource-related utilities.
- Should not contain engine or gameplay logic.

It is mainly a data layer, but it also contains the **GPU Types**, which are shared between the CPU (C++) and the GPU (GLSL).

---

## Scene

**Rules**

- Acts as the bridge between most engine modules.
- Most cross-module communication should happen through this module.
- Contains the engine-facing API.

Handles components, the ECS, game objects, and the engine API.

---

## ThirdParty

**Rules**

- Used only for build integration and includes.
- Engine code should not be modified inside this module.

Contains third-party libraries.

---

## Tools

**Rules**

- Completely independent projects.
- Have their own build system and executable or library output.
- Must not depend on any engine module.
- This rule must never be broken.

Tools that are built separately as executables or dynamic libraries and are used by the engine.

#### Reflector

See `./reflection.md`.
