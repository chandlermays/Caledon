# Caledon

**A custom 2D game engine written in C++20 — with its own physics, rendering, input, module system, and a Unity-style editor in progress.**

Caledon is a from-scratch engine built to understand what happens *underneath* tools like Unity: how a frame is stepped, how colliders find each other, how a solver resolves contacts, and how game code plugs into an engine without the engine knowing about it. It is 2D-first, with its architecture planned around adding 3D later.

Development is documented milestone by milestone in a public dev log on [LinkedIn](https://www.linkedin.com/in/chandler-mays-40385516b/).

---

## Highlights

- **2D rigid-body physics, written from scratch** — box, circle, capsule, edge and composite colliders; contact manifolds; an iterative contact solver with restitution, friction and material combine modes; sleeping; collide-and-slide; raycasts, shape casts and overlap queries.
- **Unity-style callbacks** — `OnCollision` / `OnTrigger` Enter, Stay and Exit are delivered to every Component on the colliding GameObjects.
- **Modular architecture** — the engine is a static library, the game is a separately built DLL, and the engine loads it at runtime through a small module interface.
- **GameObject / Component scene model** — local Transforms with parenting and reparenting, deferred destruction, and stable object IDs.
- **Action-based input** — input actions and action maps with axis bindings (e.g. WASD + arrow keys mapped to one `MoveHorizontal` axis).
- **Editor (in progress)** — a Dear ImGui docking shell that will host the Hierarchy, Inspector, Project and Viewport panels, Play/Stop, and JSON scene saving.

---

## Architecture

```
Caledon.slnx
├── CaledonEngine   (.lib)  Core engine: subsystems, scene model, physics, rendering, input, module loading
├── CaledonEditor   (.exe)  Editor application built on Dear ImGui (docking)
├── PacMan          (.exe)  Game host executable — boots the engine and runs the game
└── PacManModule    (.dll)  Game logic (components, input actions), loaded by the engine at runtime
```

PacMan is the first game Caledon is being built to run; the engine itself is genre-agnostic.

**Design rules the codebase follows**

- The engine is organized into **subsystems** (Window, Input, Rendering, Physics2D, Scene, Module, Logging) owned by an `EngineManager` and reached through an `EngineContext`.
- `EngineContext` is never passed down to `GameObject` or `Component` — gameplay code stays decoupled from engine plumbing.
- SDL is wrapped behind engine APIs (`CaledonEngine/API/SDL`), so SDL types stay out of engine and game headers.
- Physics math lives in pure free-function files (`ShapeQueries2D`, `ShapeDistance2D`, `ShapeManifold2D`, `ShapeCasts2D`, `ShapeMass2D`) separate from the components that use them.

---

## Physics

The physics system was built in dependency-ordered stages, each committed and verified before the next:

| Stage | What it added |
| --- | --- |
| Scaffolding | `Collider2D`, `RigidBody2D`, materials, body types, settings |
| Geometry | Shape generation, bounds, shape hashing, pure geometric queries |
| Queries | Raycasts, circle casts, overlap and spatial queries |
| Mass | Mass, center of mass and inertia per shape |
| Contacts | Contact manifolds, contact tracking, collision and trigger callbacks |
| Solver | `ContactSolver2D`, physics step integration, sleeping |
| Shapes & slide | Capsule and edge colliders, `RigidBody2D::Slide` (collide-and-slide) |

### Physics sandbox

The `PacMan` project currently boots into five demo scenes. Press **1–5** to switch scenes (pressing the active scene's key resets it).

| Key | Scene | What it shows |
| --- | --- | --- |
| 1 | Top-down | No gravity; move the player with **WASD / arrow keys** and collide with walls |
| 2 | Gravity | The player falls onto the floor |
| 3 | Bounce | Same as 2 with a bouncy material (0.8 restitution) — each bounce loses 20% speed |
| 4 | Ramp | The player rolls down a slanted ramp, drops to the floor and rolls into a wall |
| 5 | The Stack | A pyramid of boxes is knocked down by a heavy rolling ball |

> The sandbox is temporary: once the editor lands, demo scenes will be built and saved in the editor instead of in code.

---

## Building

**Requirements**

- Windows
- Visual Studio with the **Desktop development with C++** workload (toolset v145, C++20)

**Steps**

1. Clone the repository.
2. Open `Caledon.slnx` in Visual Studio.
3. Set **PacMan** as the startup project (or **CaledonEditor** to run the editor).
4. Build and run.

All third-party libraries are included in the repository — no package manager setup is needed.

---

## Roadmap

- [x] Milestone 1 — Restart plan and project structure
- [x] Milestone 2 — Rendering
- [x] Milestone 3 — Input and the engine ↔ game module bridge
- [x] Milestone 4 — 2D physics
- [ ] Milestone 5 — Editor: Project Hub, Hierarchy, Inspector, Project and Viewport panels, Play/Stop, JSON scene serialization
- [ ] Game logic for PacMan
- [ ] 3D support

Caledon 2.0 is a ground-up restart. The original version lives at [chandlermays/CaledonEngine](https://github.com/chandlermays/CaledonEngine).

---

## Third-party libraries

- [SDL3](https://github.com/libsdl-org/SDL), SDL3_image, SDL3_ttf, SDL3_mixer, SDL3_net
- [Dear ImGui](https://github.com/ocornut/imgui) (docking branch)
- [nlohmann/json](https://github.com/nlohmann/json)
- [Lua](https://www.lua.org/)

## License

[MIT](LICENSE) © 2026 Chandler Mays
