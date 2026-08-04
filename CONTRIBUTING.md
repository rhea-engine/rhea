# Contributing to Rhea

Thank you for your interest in contributing to the Rhea SDK.

Rhea is an experimental C++17 SDK under active development. Bug fixes, portability improvements, documentation updates, tests, and focused feature contributions are welcome.

## Repository Model

This GitHub repository is the public version of Rhea's main development repository, which is maintained in a private Azure DevOps workspace.

Azure DevOps is currently the source of truth. GitHub is the public release and contribution channel.

This does not prevent contributions through GitHub. Pull requests opened here are reviewed normally. Accepted changes are transferred to the main development repository and then synchronized back to GitHub.

Because of this workflow, an accepted pull request may be closed after its changes appear on the target branch instead of being merged directly through GitHub's merge button.

When transferring a contribution, the maintainers will preserve its authorship and reference the original pull request whenever possible.

## Before Opening an Issue

Please check existing issues before opening a new one.

For bug reports and build problems, include:

- operating system and version
- compiler and version
- CMake version
- graphics backend, if relevant
- Rhea commit or release
- selected CMake options
- steps needed to reproduce the problem
- expected and actual behavior
- relevant build output or logs

Remove passwords, access tokens, private paths, and other sensitive information from logs.

## Pull Requests

Keep pull requests focused on one coherent change.

Good pull requests include:

- a clear description of the problem
- an explanation of the proposed solution
- tests or reproduction steps where practical
- documentation updates when public behavior changes
- notes about platform-specific behavior
- confirmation of the configurations that were tested

Draft pull requests are welcome for early design feedback.

Large features or changes to public APIs should normally be discussed in an issue before implementation. This helps avoid work based on assumptions that do not match the project's current direction.

Avoid mixing unrelated formatting, cleanup, and functional changes in the same pull request.

## Contribution Workflow

The usual contribution flow is:

1. Fork this repository on GitHub.
2. Create a branch for one focused change.
3. Implement and test the change.
4. Open a pull request against the default branch.
5. Address review feedback on GitHub.
6. Once accepted, a maintainer transfers the contribution to the private source repository while preserving authorship.
7. The public repository is synchronized from the source repository.
8. The pull request is closed or marked as merged once the synchronized change is visible on GitHub.

The commit hashes in the synchronized repository may differ from those in the pull request because of the repository export process.

The pull request discussion remains the public review record.

## Building

The basic SDK build is:

```sh
cmake -S . -B out/build -DRHEA_BUILD_TESTS=ON
cmake --build out/build
ctest --test-dir out/build --output-on-failure
```

Some configurations require additional platform libraries or downloaded dependencies. See the root README for supported options and platform-specific requirements.

If a change affects platform or rendering code, test the relevant configuration where possible. Examples include:

- Windows with MSVC and D3D11
- Windows with OpenGL
- Linux or WSL with GCC or Clang
- SDL-enabled and SDL-disabled configurations
- builds with `RHEA_FETCH_MISSING_DEPENDENCIES` disabled

It is fine if you cannot test every platform. Clearly state what was and was not tested in the pull request.

## Code Style

Follow the style of the surrounding code.

In general:

- use C++17
- prefer clear code over clever code
- keep public APIs small and explicit
- avoid unnecessary dependencies
- keep platform-specific code isolated
- use `camelCase` for methods
- use `snake_case` for variables
- use the `m_` prefix for member variables
- add comments where intent is not obvious
- avoid unrelated formatting changes

Warnings introduced by new code should be resolved rather than suppressed unless there is a documented reason.

## Tests

Add or update tests when a change can be covered reasonably.

A bug fix should ideally include a regression test. If automated testing is impractical, provide concise manual reproduction and verification steps in the pull request.

Tests must not depend on private services, credentials, developer-specific absolute paths, or undocumented local files.

## Dependencies

Discuss new third-party dependencies before adding them.

New dependencies must have:

- a license compatible with the Rhea SDK
- an identifiable upstream source and version
- reproducible version pinning
- appropriate license and third-party notices
- a documented CMake integration strategy
- an offline or system-package path where practical

Do not commit downloaded build dependencies or generated package-manager directories.

## Generated Files

Do not edit generated files directly unless the contribution changes their generator or generation process.

If generated output must be committed, update both the generator and its output and explain how the output was regenerated.

## Licensing

Rhea is distributed under the MIT License.

By submitting a contribution, you agree that your contribution may be used, modified, redistributed, and relicensed under the same MIT License that applies to this repository.

Only submit work that you have the right to contribute. Do not include proprietary code, confidential material, or code copied from a source with an incompatible license.

## Security Issues

Do not publicly disclose vulnerabilities that could put users or systems at immediate risk.

Until a dedicated security policy and private reporting channel are published, contact the maintainers privately before opening a public issue. General hardening improvements that do not disclose an exploitable vulnerability may be submitted normally.

## Project Status

Rhea is experimental. Public APIs, file formats, package structure, and module boundaries may change as the SDK evolves.

Maintainers may decline changes that are technically valid but do not fit the current project direction or would create a maintenance burden the project cannot yet support.