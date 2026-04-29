# Contributing

Thanks for your interest in contributing!

This repository is a public, filtered export of the Poseidon renderer from the main Rhea development workspace.
The source of truth is currently an internal Azure monorepo.

## Pull Requests

Pull requests are welcome, but they are not merged directly into this repository.

If a contribution is accepted, it will be applied to the internal source repository first and then exported back to GitHub.

The usual flow is:

1. Open a pull request against this GitHub repository.
2. The change is reviewed here.
3. If accepted, the change is applied to the internal monorepo.
4. The public repository is regenerated from the monorepo.
5. The pull request is closed once the exported change appears here.

This keeps the public repository consistent with the main development workspace.

## Issues

Issues are welcome for:

- bug reports
- build problems
- documentation problems
- feature ideas
- design discussions

Please include as much context as possible, especially:

- operating system
- compiler and version
- CMake version
- commit or release version
- steps to reproduce the problem

## Code Style

Please try to follow the existing style of the surrounding code.

In general:

- prefer clear C++ over clever C++
- keep public APIs small and explicit
- avoid unnecessary dependencies
- keep platform-specific code isolated
- add comments where intent is not obvious

## Commit Scope

Please keep pull requests focused.

Good pull requests usually change one thing, for example:

- fix one bug
- improve one document
- add one small feature
- clean up one subsystem

Large unrelated changes are harder to review and may be split up.

## Generated Files

Some files in this repository may be generated during export or build steps.  
Please avoid editing generated files unless the change is specifically about generation.

## Licensing

By contributing, you agree that your contribution will be licensed under the same license as this repository.

## Project Status

The project is still evolving. APIs, package structure, and repository layout may change while the ecosystem stabilizes.