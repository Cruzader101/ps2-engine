# PS2-Style Game Engine — Architecture Plan

A planning document for a C++ game engine that runs on modern PCs while emulating
the look and the performance discipline of the PlayStation 2 era.

> **Status:** stress-tested via a `/grill-me` pass. Decisions that were previously
> unstated defaults (ECS, SIMD, an up-front cooker, swept collision) are now explicit
> choices with rationale — see **Resolved decisions** below.

## Direction

- **Platform:** Modern PC engine in C++, with PS2 constraints deliberately self-imposed
  (rather than targeting real PS2 hardware via PS2SDK).
- **Aesthetic:** *True PS2* — perspective-correct texturing, a real z-buffer, Gouraud
  vertex lighting, low internal resolution, and 16-bit dithered color. (Not the PS1
  wobble/warp look, which comes from affine texture mapping and integer vertex snapping.)
- **Color space:** light and blend in **gamma space, no linear workflow** — like the
  real PS2. This is a deliberate choice, not an oversight: gamma-space lighting gives
  harder falloff, crushed darks, and shadow-heavy gradients that make the dither read
  strongly. Linear "correctness" would sand off exactly those PS2 traits. Reversible later.

The key insight: "true PS2" is mostly a **post-processing recipe** layered on top of an
otherwise ordinary 3D renderer. Get a normal textured-and-lit scene working, then squeeze
it through a low-res dithered pipe and it instantly reads as PS2.

## Scope — what "done" means

The target is **one playable vertical slice**: walk a character around a PS2-look room —
one level mesh, one directional light, collision, one positional sound. That slice is a
falsifiable test for every feature: *does the room need it?* If not, it's deferred.

A full-game-capable engine is the eventual **outcome, not the driver.** The governing
principle:

> **Extract the engine from the slice.** Build the room demo; promote what obviously
> repeats into "the engine"; generalize only where a real second scene bends the code.
> Up-front generality — building ECS/skinning/asset pipelines for games not yet designed —
> is the specific thing that kills solo engine projects. The slice is the knife that
> decides what to say *no* to.

## Recommended stack

| Concern | Choice | Notes |
|---|---|---|
| Language | C++17 | C++20 features are a nice-to-have, not required |
| Build | CMake | |
| Window / input / audio / gamepad | SDL2 | One library covers all of it (GLFW is the windowing-only alternative) |
| Graphics API | OpenGL 3.3 core | Programmable enough for the dither shader, simple enough to avoid Vulkan boilerplate |
| GL loader | glad | Generated for GL 3.3 core, vendored into the repo |
| Textures | `stb_image` | Single-header |
| Models | `cgltf` / `tinygltf` | Load glTF source, cook to a custom binary format |
| Math | Write your own `vec` / `mat` / `quat` | Worth owning; glm is the escape hatch. **SIMD** (see decisions) |

### Toolchain & dependencies to install (Windows 11)

**Essential**
- **Visual Studio 2022 Community** (free) with "Desktop development with C++" — MSVC
  compiler + debugger. (Lighter: "Build Tools for Visual Studio 2022".)
- **CMake** — build system (cmake.org, or bundled in the VS installer).
- **Git** — version control + dependency fetching.
- **vcpkg** — C++ package manager; the clean way to get SDL2 on Windows
  (`vcpkg install sdl2`), integrates with CMake.

**Libraries** — mostly vendored files, not system installs
- **SDL2** — via vcpkg.
- **glad** — generate for GL 3.3 core from the glad web service, commit the `.c`/`.h`.
- **stb_image.h** — single header; download + commit.
- **cgltf** (or tinygltf) — needed only at the content milestone.
- **OpenGL** — nothing to download (ships with the GPU driver); keep drivers current.

**Strongly recommended**
- **RenderDoc** — free GPU frame debugger; near-essential for debugging the
  FBO/dither/RGB565 pipeline.
- **Ninja** — fast CMake build backend.

> Vendoring note: glad + single-header libs live in the repo (or via CMake
> `FetchContent`) so the build is self-contained. Only SDL2 justifies vcpkg.

## The "true PS2" rendering recipe

This is the distinctive part, and it's only a handful of moves:

1. **Render to an offscreen framebuffer (FBO)** at a low internal resolution
   (**512x448** recommended) with **RGBA8** color + depth attachments. Depth test on,
   backface culling on. Perspective-correct texturing is OpenGL's default, so the clean
   (non-warping) PS2 look is free. No sRGB framebuffer — we stay in gamma space.
2. **Gouraud vertex lighting.** Do all lighting in the *vertex* shader (a couple of
   directional lights + ambient, N·L per vertex) on the raw gamma-space colors,
   interpolate across the triangle, and just modulate the texture in the fragment
   shader. No per-pixel lighting, no normal maps — that flat, slightly soft shading is
   the PS2 signature.
3. **Dither *then* quantize in a post pass.** Sample the FBO, apply an ordered Bayer
   (4x4 or 8x8) dither, *then* quantize to RGB565. Order matters — dither first so the
   pattern breaks up the bands the quantize step creates. This produces the
   characteristic dithered gradients in skies and shadows.
4. **Upscale to the window** with nearest-neighbor. A toggleable scanline/CRT shader is
   kept **separate** — it's a CRT effect, not strictly PS2.

**Textures:** cap dimensions small (often 128 or 256), point/bilinear filtering,
**no mipmaps** (the gentle shimmer is authentic), and at cook time quantize each
texture to a ~256-color palette to mimic the PS2's CLUT/indexed textures and tight VRAM.

## Module architecture

Layers are listed top to bottom. **Dependencies point downward:** each layer may use
the ones below it but never above. The asset cooker is the one outlier — an offline
build-time tool, not part of the runtime, so it can be a separate small executable.

- **Game** *(your game)*
  - Gameplay & world
  - Camera
- **Simulation** *(engine runtime)*
  - **Scene = array-of-structs** (flat `std::vector<Entity>`; transform + mesh handle +
    optional component indices). *Not ECS* — ~20 objects don't earn it. Revisit only if
    a later scene bends under the flat layout.
  - Animation — **rigid segmented hierarchy first** (`world = parent.world * local`);
    skeletal skinning is a *later extraction*, not up-front.
  - Collision (**discrete** AABB/sphere push-out character controller — bespoke, not a
    full physics engine; swept only if something actually tunnels)
- **Engine runtime**
  - Renderer (OpenGL — RGBA8 FBO, gamma-space vertex lighting, dither/quantize post pass)
  - Audio mixer (positional sound; one uncompressed WAV first, ADPCM later if the budget bites)
  - Input mapping
- **Resources** *(infrastructure)*
  - Asset cooker — a *later extraction*: load raw glTF at runtime first, build the
    offline cooker (downsample + palette-quantize + pack) only when the 4 MB tracker fires
  - Resource manager (handles, reference counting, budget tracking)
- **Platform & core** *(infrastructure — build first)*
  - SDL2 platform (window, input, timing)
  - Math lib (`vec` / `mat` / `quat`; RH, column-major, column vectors; SIMD)
  - Memory pools (stack/frame allocator, pool allocators — no `malloc` in hot loops)

## Resolved decisions

| # | Branch | Decision | Why |
|---|---|---|---|
| 1 | End state | One playable vertical slice (walk a character around a PS2-look room) | Falsifiable test for every feature: "does the room need it?" |
| 2 | Sequencing | Slice-first, then extract toward a general engine | Up-front generality kills solo engine projects |
| 3 | Scene model | Array-of-structs, NOT ECS | Cache win unmeasurable at ~20 objects; still teaches flat layout |
| 4 | Math lib | SIMD now (deliberate learning goal) + a scalar reference oracle; don't block the cube | No measurable speed win (GPU transforms), but SIMD/VU understanding is a first-class goal |
| 5 | Memory budgets | Hard-cap arena for 32 MB main; accounting layer for 4 MB "VRAM" (tally `glTexImage2D`, assert past 4 MB) | The two budgets need different mechanisms; VRAM is the driver's, can't arena it |
| 6 | Math conventions | RH, column-major, column vectors (`Proj*View*Model*v`, upload `GL_FALSE`) | OpenGL-native; no transpose, CPU `Mat4` matches GLSL `mat4` |
| 7 | Asset cooker | Deferred; raw glTF at runtime first, build cooker when the 4 MB tracker fires | Don't design the binary format before the renderer says what it needs |
| 8 | Character | Rigid segmented hierarchy first; skinning later | Teaches hierarchy, more PS2-authentic (joint gaps); hierarchy reused by a real skeleton |
| 9 | Game loop | Fixed timestep + accumulator + render interpolation | Deterministic, framerate-independent, stable collision. **Deferred at skeleton** — a simple vsync-capped loop ships milestone 1; the accumulator lands when the first sim (character + collision) needs it. See ADR-0002 |
| 10 | Color pipeline | Gamma space, no linear workflow, RGBA8 FBO | More authentic AND simpler; makes the dither read strong; reversible later |
| 11 | Collision | Discrete AABB/sphere push-out first; swept only on real tunneling | Fixed dt caps per-step motion; swept's edge cases are a slice-staller |
| 12 | Audio | One uncompressed positional WAV via SDL2; ADPCM deferred | ADPCM is gold-plating until the 32 MB budget makes sample size hurt |
| 13 | Error handling | Three-class taxonomy: bug → `assert`-and-die; environmental → value-based error + log (abort or skip); unrecoverable → log + terminate. **Exceptions off.** See ADR-0001 | Bugs must crash at their source, not be silently survived; boundary conditions handle locally, so nothing needs exception unwinding — gain determinism + no hot-path surprises |

## Build order

A sane sequence that always leaves you with something running:

1. **Skeleton** — CMake + SDL2 + glad, open a window, clear it to a color. Proves the toolchain.
   **✅ Done (2026-07-11).** `src/main.cpp`: GL 3.3 core context, glad loaded, vsync-capped
   render loop clearing to blue, `PS2_ASSERT`/`LOG` macros, reverse-order teardown. Loop
   pacing recorded in ADR-0002. See dev-log 2026-07-11.
2. **First geometry** — your math lib plus a hardcoded spinning textured cube with a
   perspective camera, rendered straight to the screen. Write vec/mat *scalar first* to
   get the cube up; **then** rewrite them as SIMD, keeping the scalar version as a test
   oracle to diff against.
3. **The PS2 look** — add the RGBA8 FBO, the Bayer-dither-then-RGB565 post pass, and the
   nearest-neighbor upscale. The payoff milestone: the same cube suddenly reads as PS2.
4. **Budgets** — wire the arena allocator (32 MB, hard fail) and the VRAM accounting
   tracker (4 MB, assert) in *now*, so the discipline forms early rather than being
   retrofitted onto bad habits.
5. **Content** — load raw glTF at runtime with the budget tracker firing; when the 4 MB
   tracker goes red, build the offline cooker (downsample + palette-quantize + pack).
   Materials with Gouraud vertex lighting.
6. **World** — array-of-structs scene plus the frame allocator, multiple objects, a
   free-fly camera you later constrain into a game camera.
7. **Slice close** — rigid segmented character, discrete collision, the fixed-timestep
   loop, one positional WAV, and a debug overlay. **Done = you can walk around the room.**

## The discipline that matters

The memory budget isn't decoration. Wire up the allocators and the **32 MB main /
4 MB texture** trackers early and let them yell at you. That constraint is what turns
this from "a renderer with a filter" into something that teaches the data-oriented
discipline the PS2 was famous for: budgeting, packing, batching draw calls, and
minimizing state changes — the PC analog of the PS2's DMA-chain mindset.

The 32 MB budget is a real arena in system RAM (`alloc()` fails loudly when full). The
4 MB "VRAM" budget is an accounting layer — every texture upload adds its byte size to a
counter, and the upload that would cross 4 MB asserts. Different plumbing, same teeth.
