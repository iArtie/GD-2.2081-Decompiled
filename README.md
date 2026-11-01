# GD-2.2081-Decompiled

C++ reconstruction of Geometry Dash 2.2081, as close as possible to the original binary. The repository contains 1,216 files (608 `.h` and 608 `.cpp`) under `src/`.

## Tools

- IDA 9.1
- [BromaIDA](https://github.com/Stazzical/BromaIDA) (Stazzical): Broma support for IDA
- [GD Web Decomp](https://gd-src.undefined0.dev/)

`GJEnums.h` (`src/Utils/GJEnums.h`) was not reconstructed from the binary. It was taken from the [Geode bindings](https://github.com/geode-sdk/bindings/tree/main/bindings) (`include/geode/enums.hpp`).

## Project idea

The project started when BromaIDA ported the class members into IDA. With members already typed per class, decompilation went from impractical to tractable.

On top of that, a script (since lost, but trivial to recreate) was written to simulate RobTop's original code organization:

- It merged all methods of a class into a single `.cpp`. For example, `CreatorLayer::onTreasureRoom` and `CreatorLayer::onDaily`, which are independent functions in IDA, ended up in `CreatorLayer.cpp`.
- It collected the member variables used by each class and generated the `.h` with its includes, inheritance and members.

Limitation: a script cannot reliably infer whether a member is an `int`, a `bool`, an array, etc. The generated headers contained every member of the class, but typed as `void*`, so each one had to be identified and fixed by hand.

The extraction also produced classes that are probably useless. All of them were reconstructed anyway, which is why some files look unusual.

## Assisted reconstruction

Transcription and verification of the pseudocode was done with an LLM. Even with typed members, decompiler output carries an excessive amount of residue (STL wrappers as `sub_*`, inlined or lost variables), so the prompt includes automatic substitution rules.

Recognized functions:

| Original function | Replacement |
|---|---|
| `sub_D153B8` | `std::string` literal construction |
| `sub_D18D28` | String copy |
| `sub_D18EA8` | String assignment |
| `sub_D163D4` | String move (`std::move`) |
| `sub_D15E14` | String concatenation |
| `sub_D1641C` | `string::find` |
| `sub_D1861C` | `string::replace` |
| `sub_D14F84` | `string::substr` |
| `sub_8265A8` | Replace all occurrences |
| `sub_827328` | String split |
| `sub_CFAB14` | `std::next(it)` |
| `sub_CFABD4` | `std::prev(it)` |
| `sub_D0BBC8` | `time(nullptr)` |
| `sub_620B38` | Frame retrieval via `CCDirector` |
| `sub_5BB878` | SHA-1 block processing |
| `sub_6210AC` | Lookup in `std::unordered_set` |
| `sub_62116C` | Lookup in `std::unordered_map` |

Member type inference:

| Decompiled type | Interpretation |
|---|---|
| `BYTE` | `bool` when it represents flags |
| `DWORD` | `int` or `float`, depending on usage |
| `QWORD` | Pointer to an object |

## Manual review

The LLM output was reviewed by hand. Many methods were missing and others were skipped because of their length, so they had to be reconstructed again. The largest classes were the most affected: `GJBaseGameLayer` went from over 40k decompiled lines to 13k after cleanup, and `PlayerObject` and `GameObject` went through an equivalent process.

Each class was checked against the decompiled code for similarity. However, in several cases the amount of residue made it preferable to reconstruct the logic differently rather than transcribe it literally. The result is therefore not 1:1, but it is the closest available.

## Compilation

The project has never been compiled.

Several classes will probably compile unchanged when integrated into a project using RobTop's modified cocos2d-x 2.2.3. The large classes, such as `GJBaseGameLayer`, are likely incomplete or incorrectly reconstructed: the larger the class, the larger the margin of error.

## Structure

All `#include` paths are relative to `src/`; add `src` to the include directories.

| Folder | Contents |
|---|---|
| `Layers/` | `*Layer` |
| `Popups/` | `*Popup` |
| `Popups/Setup/` | `Setup*` (trigger and object options) |
| `Managers/` | `*Manager`, `GManager` |
| `Delegates/` | `*Delegate`, `*Protocol` |
| `Cells/`, `Pages/`, `Nodes/`, `Sprites/` | `*Cell`, `*Page`, `*Node`, `*Sprite` |
| `Objects/` | `GameObject` and derivatives, triggers, `PlayerObject`, effect actions |
| `Models/` | Level, user and server data |
| `Editor/` | `EditorUI`, transform controls, HSV and colors, smart blocks |
| `Cocos/` | `CC*` extensions |
| `Audio/` | `FMOD*` and music/SFX browsers |
| `UI/` | Generic widgets |
| `Utils/` | Helpers, enums and states |
| `Platform/` | Runtime and library stubs (`std`, `cocos2d`, `fmt`, `pugi`, `llvm`), `ThirdParty/` |

## Notes

With over 600 classes, some errors have almost certainly slipped through. The LLM tended to leave comments in the code, so leftovers may remain.

Note on project structure:** The folder layout under `src/` is not the original Geometry Dash source organization. GitHub does not display more than 1,000 files per directory, so the files were split into folders by class type (Layers, Managers, Popups, etc.). This structure is my own and was chosen only for browsability. Includes were rewritten accordingly and are relative to `src/`.

Any contribution, correction or pull request is welcome.