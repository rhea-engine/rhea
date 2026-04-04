# Rhea Engine Naming Guide

## Overview

This document defines the naming conventions for the **Rhea engine
ecosystem**.

### Core Principles

-   **Meworla** → Company / legal ownership
-   **Rhea** → Engine / ecosystem / developer-facing platform
-   **mwpkg** → Package manager / tooling

The goal is to ensure: - Consistency across code, packages, and
tooling - Clear distinction between company and product - Scalability
for future ecosystem growth

------------------------------------------------------------------------

## 1. Package Naming

### Format

    <namespace>/<name>

### Official Packages

Use the `rhea` namespace:

-   `rhea/core`
-   `rhea/client`
-   `rhea/editor`
-   `rhea/rendering`
-   `rhea/physics`
-   `rhea/networking`

### Third-party Packages

Use custom namespaces:

-   `acme/dialogue`
-   `janedoe/inventory`
-   `studiox/steam`

### Rules

-   Use lowercase
-   Use clear, descriptive names
-   Avoid product prefixes (e.g. avoid `rhea/rhea-client`)

------------------------------------------------------------------------

## 2. C++ Namespaces

### Format

``` cpp
namespace rhea {}
namespace rhea::client {}
namespace rhea::physics {}
```

### Rules

-   Always lowercase
-   Mirror package structure
-   Do not include company name (`Meworla`)
-   Avoid concatenated names like `RheaClient`

### Example

``` cpp
namespace rhea::client
{
    class ClientApp;
}
```

------------------------------------------------------------------------

## 3. Include Paths

### Format

``` cpp
#include <Rhea/...>
```

### Examples

``` cpp
#include <Rhea/Engine.h>
#include <Rhea/Client/ClientApp.h>
#include <Rhea/Physics/PhysicsWorld.h>
```

### Rules

-   Root is PascalCase: `Rhea`
-   Subsystems are folder-based
-   Must align with namespace structure

------------------------------------------------------------------------

## 4. CMake Targets

### Format

    Rhea::<Module>

### Examples

-   `Rhea::Core`
-   `Rhea::Client`
-   `Rhea::Physics`
-   `Rhea::Rendering`

### Usage

``` cmake
target_link_libraries(MyGame PRIVATE Rhea::Core Rhea::Physics)
```

------------------------------------------------------------------------

## 5. Repository Naming

### Format

-   Core: `rhea`
-   Modules: `rhea-<name>`

### Examples

-   `rhea`
-   `rhea-client`
-   `rhea-editor`
-   `rhea-physics`
-   `rhea-rendering`

### Rules

-   lowercase
-   dash-separated
-   align with package id

------------------------------------------------------------------------

## 6. Package Metadata (`module.json`)

### Example

``` json
{
  "id": "rhea/client",
  "version": "0.9.0",
  "displayName": "Rhea Client",
  "author": "Meworla",
  "license": "Apache-2.0",
  "moduleClass": "rhea::client::ClientModule",
  "dependencies": {
    "rhea/core": "0.9.0"
  }
}
```

### Rules

-   `id` is canonical identifier
-   `author` stores company (Meworla)
-   `displayName` is user-facing
-   `moduleClass` uses full namespace

------------------------------------------------------------------------

## 7. CLI Usage (mwpkg)

### Examples

``` bash
mwpkg add rhea/core
mwpkg add rhea/client
mwpkg add acme/dialogue
```

### Rules

-   Always use package ids
-   Keep names short and readable
-   Avoid company prefixes in CLI

------------------------------------------------------------------------

## 8. Class Naming

### Rules

-   Use PascalCase
-   Do not repeat namespace in class name

### Example

Bad:

``` cpp
class RheaClientApp;
```

Good:

``` cpp
namespace rhea::client
{
    class ClientApp;
}
```

------------------------------------------------------------------------

## 9. Do's and Don'ts

### Do

-   Use `rhea/...` for official modules
-   Keep namespaces lowercase
-   Keep includes consistent
-   Keep naming aligned across systems

### Don't

-   Use `Meworla/...` as package ids
-   Use uppercase namespaces
-   Duplicate names across layers (`RheaClientClient`)
-   Mix old and new conventions

------------------------------------------------------------------------

## 10. Migration Guide

### Package IDs

-   `Meworla/Rhea` → `rhea/core`
-   `Meworla/RheaClient` → `rhea/client`
-   `Meworla/Poseidon` → `rhea/networking`

### Namespaces

-   `Rhea` → `rhea`
-   `RheaClient` → `rhea::client`

### Includes

-   `RheaClient/...` → `Rhea/Client/...`

### CMake

-   `RheaClient` → `Rhea::Client`

------------------------------------------------------------------------

## 11. Guiding Philosophy

> Developers interact with **Rhea**, not Meworla.

-   Meworla = owner
-   Rhea = platform
-   mwpkg = gateway

This separation ensures: - Strong ecosystem identity - Clean developer
experience - Long-term scalability

------------------------------------------------------------------------

## 12. Summary

  Layer        Naming
  ------------ -----------------
  Company      Meworla
  Engine       Rhea
  Package ID   rhea/client
  Namespace    rhea::client
  Include      Rhea/Client/...
  CMake        Rhea::Client
  Repo         rhea-client

------------------------------------------------------------------------

End of document.
