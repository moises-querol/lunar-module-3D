# Lunar Module 3D — Product and Technical Design

Date: 21 October 2026  
Status: design proposal grounded in the current repository  
Language: English for code, filenames, comments, and interface text

Implementation note: the browser application is implemented in `html/`, alongside the native application. Per the project owner's updated requirement, it loads the existing `assets/textures/*.tga` files through Three.js `TGALoader` and Vite URL imports rather than keeping converted image copies. The PNG conversion and separate browser asset folder proposed below are superseded by that requirement. Production assets in ignored `html/dist/` are generated distribution output. See the README for current commands.

## 1. Purpose

Lunar Module 3D is an interactive viewer of a stylized Apollo 11 Eagle lunar module. Users inspect its structure by rotating, moving, and zooming the scene, and change the terrain and displayed flag. The repository identifies the original application as a computer graphics coursework project from 2007.

The project demonstrates geometric modeling, transformations, lighting, materials, textures, and mouse interaction. It is a visual educational application, not an engineering model or a flight simulator. Its dimensions, planetary appearances, materials, and structural details should not be presented as historically or physically exact.

This document describes the existing implementation and proposes a small modern replacement. Proposed behavior is explicitly distinguished from implemented behavior. Creating this document does not migrate the application or change its runtime.

## 2. Product goals and boundaries

The modern version should make the model easy to view, distribute, and maintain while preserving its recognizable silhouette and existing scene choices.

Primary goals:

- Present a clear, solid model whose cabin, gold base, legs, and attachments are easy to distinguish.
- Preserve Moon, Mars, and Venus environments and USA, Spain, and USSR flags.
- Provide predictable camera controls and discoverable interface controls.
- Replace legacy graphics calls with supported GPU rendering through a small graphics library.
- Keep geometry, materials, and application behavior understandable without building a general-purpose engine.
- Use one application, one asset collection, and a short list of dependencies.

The initial modernization does not include flight dynamics, gravity simulation, landing gameplay, animated astronauts, editable CAD geometry, accounts, multiplayer, a database, or a server API. Component labels, exploded views, and animations are possible later additions rather than baseline requirements.

## 3. Current application

### 3.1 Scene contents

| Component | Existing representation |
| --- | --- |
| Cabin | Faceted panels defined directly in C, including upper and lower attachments |
| Descent base | Gold octagonal body with narrow upper and lower bevels, plus underside and side cylinders |
| Landing assembly | Four repeated leg assemblies with main struts, braces, joints, and shallow circular footpads |
| Access structures | Ladder on one landing leg and a ramp with handrails |
| Flag | Textured rectangular cloth and a separate pole |
| Terrain | A flat textured plane beneath the model |
| Lighting | Directional key, fill, and rim lights with a low global ambient contribution |
| Materials | Gold, aluminum, copper, and a neutral material for textured surfaces |

The cabin uses explicitly triangulated panels, triangle-derived normals, and dark panel outlines. Cabin rendering enables lighting on both sides. Most rounded parts use procedural surfaces of revolution. The base now uses separate octagonal bands with face normals, and the footpads have a small amount of thickness.

### 3.2 Implemented features

| Feature | Current behavior |
| --- | --- |
| Initial scene | Moon terrain and USA flag |
| Rotation | Left mouse drag applies a custom virtual-trackball rotation to the entire scene |
| Zoom | Middle mouse drag changes the scene's depth translation |
| Pan | Shift + middle mouse drag changes horizontal and vertical translation |
| Environment selection | Right-click menu selects Moon, Mars, or Venus |
| Flag selection | Right-click menu selects USA, Spain, or USSR |
| Background | Black for Moon, reddish brown for Mars, pale yellow for Venus |
| Resize | Viewport, perspective aspect ratio, and trackball parameters update with window size |
| Exit | Escape terminates the application |
| Texture failure | Failed startup textures prevent launch; a failed later selection retains the previous texture |

Environment selection changes the ground texture and background color. It does not change terrain geometry, gravity, the model, or the lighting preset. Flags are independent visual options; they do not imply that a corresponding mission visited the selected planet.

### 3.3 Startup and rendering flow

1. Create a 700 × 700 GLUT window with double buffering and a depth buffer.
2. Register keyboard, mouse, motion, display, and resize callbacks.
3. Configure a 50-degree perspective projection with near and far planes of 0.1 and 10 scene units.
4. Load the Moon and USA textures.
5. Compile the scene's drawing commands into one OpenGL display list.
6. Initialize lighting and attach the right-click menu.
7. Enter the GLUT event loop.

Each redraw clears the frame and depth buffer, applies the current translation and trackball matrix, executes the display list, and swaps buffers. Geometry remains static. Texture selection updates the existing ground or flag texture object and requests another redraw.

The initial scene translation is four units away from the viewer. Zoom is translation-based and is not explicitly bounded, so users can move the scene into clipping ranges. There is no reset-view action or visible control panel.

### 3.4 Source responsibilities

| File | Responsibility |
| --- | --- |
| `src/main.c` | Window, callbacks, camera-like scene transforms, projection, menus, and display-list lifecycle |
| `src/scene.c` | Primitive geometry, cabin panels, transforms, model assembly, terrain, flag, ladder, and ramp |
| `src/math_utils.c` | Vectors, matrices, normalization, and trackball rotation calculations |
| `src/lighting.c` | Fixed-function light configuration |
| `src/materials.c` | Fixed-function material parameters |
| `src/textures.c` | Restricted TGA decoding, texture upload, and texture-slot binding |
| `Makefile` | Native compilation, platform-specific linking, dependency detection, and cleanup |
| `assets/textures/` | Six terrain and flag TGA images |
| `assets/images/` | Example screenshots; these are not used to render the application |

Public headers expose the corresponding module entry points. `scene.c` is the largest source file and mixes geometry data with rendering operations. Many cabin functions call an earlier function under a local transform, making the final mesh difficult to understand from any single function.

### 3.5 Current technology and constraints

The application uses C, GNU Make, OpenGL compatibility functionality, GLU, and GLUT/freeglut. The Makefile supports Linux and contains macOS build paths, but macOS behavior is not established by the repository. There is no Windows-specific build configuration.

The graphics path relies on `glBegin`/`glEnd`, matrix-stack operations, fixed-function lighting and materials, and display lists. These mechanisms require a compatibility implementation and cannot be used directly in a modern OpenGL core renderer or a browser WebGL context. The limitation is this rendering path, rather than the general concept of using a GPU or OpenGL.

Textures must be complete 256 × 256, uncompressed, 24-bit TGA images with a bottom-left origin. The loader changes byte ordering to retain the project's historical mapping, and sampling uses nearest filtering. Asset paths are relative to the working directory, so the executable is normally launched from the repository root.

Other limitations include no cast shadows, flat terrain, hardcoded material values and placement offsets, and extensive shared rendering state. The application has no persistence, physics engine, networking, model import, or graphical settings interface.

## 4. Recommended technology

### 4.1 Recommended direction: a browser-based viewer

Use **TypeScript, Three.js, and Vite**, with a small HTML/CSS interface. This is a product recommendation based on the current viewer's size and behavior: distributing a static browser application is a good fit for a model that has no native integrations or server-side requirements.

A browser target is an assumption in this proposal, not an existing user requirement. If preserving a native C coursework application or teaching low-level graphics is the primary objective, a native shader-based renderer would be a better direction. That alternative would retain a compiled desktop workflow and require explicit GPU buffers, shaders, window/input integration, and packaging. It should be chosen before beginning migration, rather than developed alongside the browser implementation.

| Area | Recommended technology | Reason |
| --- | --- | --- |
| Application language | TypeScript with strict checking | Clear types for scene choices, configuration, and resource ownership |
| Graphics | Three.js `WebGLRenderer` | GPU rendering and a scene graph without maintaining a custom rendering engine |
| Graphics baseline | WebGL 2 | Required by the current Three.js WebGL renderer; detect support at startup |
| Camera interaction | Three.js `OrbitControls` | Existing orbit, zoom, pan, and touch behavior |
| Materials | `MeshStandardMaterial` | A metalness/roughness material model suitable for the base and structural parts |
| Interface | Semantic HTML and ordinary CSS | Enough for two selectors, a reset action, and a help panel |
| Development and bundling | Vite | Development server, asset handling, and static production output |
| Geometry initially | Three.js primitives and explicit `BufferGeometry` | Preserve the current procedural model with a small migration |
| Optional authored model format | glTF 2.0 / GLB through `GLTFLoader` | Separate authored geometry from application code when useful |
| Image assets | PNG initially; WebP where appropriate | Standard browser decoding and predictable texture handling |
| Package management | npm with a committed lockfile | One reproducible dependency workflow |
| Distribution | Static files served over HTTPS | The viewer needs no runtime backend |

The current Three.js renderer uses WebGL 2. `OrbitControls` maintains an up direction, so it is intentionally different from the legacy free trackball. The material and loader capabilities above are documented in the official [renderer](https://threejs.org/docs/pages/WebGLRenderer.html), [controls](https://threejs.org/docs/pages/OrbitControls.html), [materials](https://threejs.org/docs/pages/MeshStandardMaterial.html), and [glTF loader](https://threejs.org/docs/pages/GLTFLoader.html) references.

Vite handles TypeScript transformation, but the production build should explicitly run `tsc --noEmit` before `vite build` so type checking is part of the build. Its [feature documentation](https://vite.dev/guide/features.html) and [static deployment guide](https://vite.dev/guide/static-deploy.html) describe the development and distribution workflow.

Use compatible stable package releases and record exact resolved dependencies in the lockfile when implementation starts. Do not choose versions solely from this document's date. Node.js is a development/build dependency, not a server required by the deployed application.

React, a state-management library, a physics engine, and custom shaders are unnecessary for the initial feature set. WebGPU can be evaluated later if a concrete capability requires it; it does not need to be the initial rendering baseline.

## 5. Modern product behavior

### 5.1 Primary user journey

The user opens the viewer, sees the complete module on Moon terrain with the USA flag, inspects it with mouse or touch controls, optionally changes the location or flag, and resets the view when needed. A short loading message appears while essential resources load. Errors appear as readable text with a retry action where recovery is possible.

### 5.2 Initial feature scope

| Capability | Requirement | Relationship to current application |
| --- | --- | --- |
| Model viewer | Show the cabin, attachments, beveled base, legs, pads, ladder, ramp, terrain, and flag | Preserve |
| Camera | Orbit around a stable model-centered target, zoom, and pan | Modernize interaction |
| Location selector | Moon, Mars, and Venus, updating terrain and background together | Preserve |
| Flag selector | USA, Spain, and USSR, independent of location | Preserve |
| Reset view | Restore the initial framing and camera target without changing selections | New |
| Visible controls | Compact labeled selectors and a help toggle | New; replaces hidden context menus |
| Resize | Correct aspect ratio and canvas resolution within the page layout | Preserve and extend |
| Touch input | One-finger orbit, two-finger zoom and pan | New |
| Loading and error UI | Show essential loading state and retain valid selections on optional asset failure | Extend |
| Unsupported graphics | Explain that WebGL 2 is unavailable instead of showing an empty canvas | New |

Default desktop input should use left drag to orbit, wheel or middle drag to zoom, and right drag to pan. The page's visible controls replace the GLUT right-click menu. This is a deliberate interaction change. Provide keyboard actions to rotate, zoom, pan, and reset while the canvas is focused, with bindings shown in the help panel. Form controls must keep their normal keyboard behavior.

Escape should dismiss an open help panel or leave fullscreen if fullscreen is later added. It should not attempt to close the browser tab. A web page does not need a native-style Quit command.

### 5.3 Scope discipline

The first modern release should preserve model detail rather than redesign the spacecraft. The recent cabin closures, base bevels, and thicker footpads are part of the reference appearance. Additional panel seams, foil textures, component labels, screenshots, auto-rotation, and saved preferences can be considered after feature parity.

## 6. Visual and rendering design

### 6.1 Geometry

Use a consistent Y-up coordinate system and a documented local origin at the module's center on the ground plane. The existing coordinates are arbitrary scene units; retain the relative proportions during migration and avoid presenting them as real-world dimensions.

Model ownership should follow recognizable physical parts: cabin, descent base, landing assembly, access structures, and flag. Each part has local geometry and a single explicit placement transform. Repeated legs and braces should share geometry and materials where their shapes match.

All solid exterior surfaces should have intentional triangulation, consistent outward winding, and appropriate normals. Keep sharp edges at cabin panels and octagonal base corners; use smooth normals on cylinders and spheres. Do not average normals across every shared vertex indiscriminately. Use double-sided surfaces only where appropriate, such as flag cloth, rather than to conceal missing or reversed solid faces.

Use a common ring vertex calculation for each base band and cap so beveled edges meet exactly. Preserve the pad diameter and ground contact while retaining its shallow thickness. Avoid coplanar duplicate faces and excessive outline geometry, particularly around intersecting cabin attachments.

### 6.2 Materials and lighting

Use separate shared materials for gold panels, aluminum cabin surfaces, copper-colored struts, matte terrain, and flag fabric. Convert the current visual palette deliberately; legacy shininess values are not direct replacements for roughness.

Start with moderately rough metallic surfaces so the structure is legible. Metals need environment illumination to remain visible, so pair a restrained environment map with a directional key light and a small fill contribution. The Three.js [standard material reference](https://threejs.org/docs/pages/MeshStandardMaterial.html) recommends environment mapping for best results. The initial implementation can use a generated studio environment rather than introducing a large external HDR asset.

Use one consistent output color pipeline and tone mapping. Mark color textures as sRGB and keep any later normal or roughness maps in their appropriate non-color space. Avoid compensating for incorrect normals by increasing ambient brightness.

Limit shadows to one key light, with the module casting onto the ground. Keep the shadow region close to the model. Retain a small readable contact shadow under the pads and base. These are proposed visual improvements; the current implementation has no shadow map.

Environment choices retain the existing stylized palettes. The initial version should keep the same neutral inspection lighting across locations, making material and geometry comparisons consistent. Planet-specific atmospheric effects would be a separate feature.

### 6.3 Camera and presentation

Frame the entire module, flag, and footpads on initial load. Keep the rotation target near the module rather than at the origin of the whole terrain plane. Bound camera distance to prevent clipping into the cabin or losing the model far away. Derive the clipping range and framing from model bounds rather than carrying over the legacy constants blindly.

Prefer an upright orbit camera. Document this difference from the existing free trackball. If unrestricted rolling becomes an explicit requirement, evaluate a trackball controller instead of combining two interaction systems.

A neutral page background and compact control panel should leave most space to the model. Give selectors visible labels, show the active choices, preserve keyboard focus indicators, and include a concise textual scene description for users who cannot inspect the canvas visually.

## 7. Assets and geometry migration

Start by reusing the current assets and model design. The original TGA files are small and usable as conversion sources; replacing them with larger textures does not automatically improve their detail.

1. Convert terrain and flag TGA files to PNG while preserving their displayed orientation and colors.
2. Record texture orientation explicitly. The current decoder's byte reversal is a legacy mapping convention and should not be copied blindly into a browser loader.
3. Flatten the cabin's nested transforms into explicit part-local vertices and triangle indices. Preserve known closures and recessed areas.
4. Recreate cylinders, spheres, struts, bevels, and footpads with standard geometry or small procedural builders.
5. Assign material groups and calculate normals according to hard and smooth edges.
6. Keep editable geometry definitions separate from placement logic.

A GLB file is an optional later consolidation, not a prerequisite. If geometry is authored in a modeling tool, keep its editable source alongside the exported runtime file and preserve component names. `GLTFLoader` loads glTF 2.0 assets; it does not automatically convert immediate-mode C drawing commands into a model.

Retain original asset attribution when available. The current repository does not establish a complete rights history for every texture or screenshot, so public redistribution should use assets with recorded permission or suitable replacements.

## 8. Application architecture

### 8.1 Proposed repository layout

This is the target layout for the modern application, not a change already applied to this repository:

```text
src/
  main.ts               Startup, selection state, and application coordination
  viewer.ts             Renderer, camera, controls, resize, and frame scheduling
  scene.ts              Scene assembly, lights, ground, and part placement
  model.ts              Model parts and reusable primitive construction
  cabin.ts              Cabin mesh data and explicit panel construction
  materials.ts          Shared materials and their visual parameters
  assets.ts             Asset loading, caching, and resource disposal
  presets.ts            Typed location and flag definitions
  ui.ts                 Selectors, reset action, help, and status messages
  styles.css            Page layout and control styling
public/
  assets/
    textures/           Converted terrain and flag images
    models/             Optional authored GLB and editable model source
index.html
package.json
package-lock.json
tsconfig.json
DESIGN.md
README.md
```

Avoid creating a file for every cabin panel or primitive. Extract another module only when it has a distinct responsibility. Do not add an engine abstraction, service layer, dependency-injection framework, or plugin system for this scene.

### 8.2 State and ownership

Keep application state small: selected environment, selected flag, and loading/error status. Camera state belongs to the viewer/controller, rather than duplicating all camera matrices in a separate store. A selector event requests a resource change; it does not rebuild the model.

`viewer.ts` owns the renderer, camera, controls, and canvas lifecycle. `scene.ts` owns scene objects and lighting. `assets.ts` owns shared texture resources. Geometry and materials are reused across repeated parts, and disposal must occur once after all consumers release a shared resource.

Environment presets contain a stable identifier, display label, texture URL, and background color. Flag presets contain an identifier, label, and texture URL. Resolve URLs relative to the configured application base so deployment beneath a URL subdirectory works.

### 8.3 Selection and loading behavior

On startup, load the default environment and flag before displaying the complete scene. Remaining textures may load on demand and be cached. An environment's texture and background should change together after the texture is ready.

Keep the previous selection active until replacement assets load. If the user changes choices rapidly, identify requests by sequence or selection ID so an older completion cannot overwrite a newer choice. A failed request leaves the previous valid appearance visible and produces a readable error message.

### 8.4 Frame and resource lifecycle

Use a single render scheduler. Redraw on camera changes, resize, successful asset changes, and relevant interface actions. If control damping is enabled, continue frames while motion settles, then return to idle. Pause unnecessary work when the page is hidden.

Handle container resizing with `ResizeObserver`, updating both camera aspect ratio and renderer size. Cap device pixel ratio at 2 initially to bound GPU work on dense displays. Dispose geometry, textures, materials, controls, and event listeners when their owner is destroyed. Display a recoverable status if the graphics context is lost.

## 9. Performance and operational requirements

The scene is small enough that straightforward meshes and shared materials should be sufficient. Reuse the four leg assemblies and their geometry before adding specialized instancing. Avoid generating geometry or allocating large objects during camera movement.

Proposed budgets are design targets, not measured properties of the existing application:

| Area | Initial target |
| --- | --- |
| Desktop interaction | Approximately 60 frames per second at a 1920 × 1080 canvas on a representative integrated GPU |
| Modest mobile device | Approximately 30 frames per second with restrained resolution and shadows |
| Initial runtime payload | Aim below 5 MB of compressed JavaScript and default scene assets |
| Texture dimensions | Retain the original 256 × 256 sources initially; increase only with a useful replacement |
| Idle operation | Stop continuous rendering when nothing changes |
| Asset switching | Replace textures without recreating geometry or accumulating GPU resources |

Use a clear unsupported-browser message when WebGL 2 cannot initialize. Keep controls usable at narrow widths and avoid requiring a middle mouse button. Persisting preferences is optional; the initial version can reset to its defaults on each load.

The baseline application processes no personal data and sends no user-generated content to a service. Keep assets local to the deployed site and avoid adding external runtime scripts or analytics without a product reason.

## 10. Development and distribution

The proposed developer workflow uses `npm ci` to install locked dependencies, `npm run dev` to start local development, and `npm run build` to run TypeScript checking and create production assets. Define these scripts when implementing the new application; they are not available in the current C repository.

Vite's default production output is `dist/`. Treat it as generated output and ignore it in version control, along with `node_modules/`. The current `build/` directory holds native intermediate files; it is a separate artifact and is still needed by the existing Makefile workflow.

Serve the production output as a static site over HTTPS. Configure the correct base URL when hosting beneath a path. `vite preview` is for local build previews, not a production server, as described in the official [deployment guide](https://vite.dev/guide/static-deploy.html).

No hosting provider is required by this design. An ordinary static file host is sufficient. A browser viewer still needs its files served over HTTP/HTTPS; opening `index.html` directly from the filesystem is not the supported distribution workflow.

## 11. Migration sequence and completion conditions

### Phase 1 — Capture the model and establish the viewer

Preserve the native application as the reference while creating the TypeScript/Vite shell. Establish the camera, input, page controls, loading state, and renderer. Recreate the model with explicit meshes and convert the six texture assets.

Completion means the full scene is represented, its primary parts have clear ownership, and the default view frames the module and flag. The legacy application continues to be available until the replacement is ready.

### Phase 2 — Reach feature parity

Connect all environment and flag choices. Implement resize behavior, camera limits, reset view, and asset failure handling. Preserve the recognizable palette, cabin closures, base bevels, leg arrangement, ladder, ramp, and footpad placement.

Completion means all nine environment/flag combinations are representable, selections remain independent, and repeated changes do not rebuild the model or lose a valid appearance on load failure. The documented camera behavior should match the chosen modern controls rather than accidentally imitate the old transform stack.

### Phase 3 — Finish the visual presentation

Tune material roughness, environment illumination, key lighting, contact shadows, texture filtering, and sparse cabin edges. Establish responsive controls and the idle render lifecycle. Resolve visible gaps or duplicate surfaces in the actual mesh rather than concealing them with broad double-sided rendering.

Completion means the cabin and base read clearly from normal viewing angles, pads meet the ground, and lighting shows the intended hard and smooth edges without overwhelming outlines.

### Phase 4 — Publish the replacement

Document the final commands and controls in the README, record asset provenance, and prepare the static output. Replace the native source tree only after the modern application satisfies the agreed scope and the legacy version has a recoverable repository reference.

Completion means the selected distribution location serves the application and its assets correctly. Publishing and removing native source are later implementation actions, not actions performed by this document.

## 12. Principal design decisions and risks

| Decision or risk | Consequence and treatment |
| --- | --- |
| Browser distribution is proposed | Confirm that preserving native C is not the primary educational requirement before migration begins |
| Cabin geometry is embedded in nested C functions | Extract explicit coordinates and triangles; copying draw-call ordering alone can repeat missing or overlapping faces |
| Legacy and modern cameras behave differently | Use a documented upright orbit by default; do not claim exact trackball parity |
| Legacy colors do not map directly to PBR | Tune materials and environment illumination together while preserving the recognizable palette |
| Existing texture orientation is unusual | Preserve the visible result during conversion and make the new UV convention explicit |
| Metallic surfaces can appear too dark | Supply restrained environment illumination rather than relying solely on a direct light |
| Shadows can hide structure or consume GPU time | Use one bounded shadow map and conservative quality settings |
| A complete rewrite can expand unnecessarily | Finish viewer parity before adding labels, motion, extra textures, or a more elaborate interface |

## 13. Design basis

Existing behavior is derived from `src/main.c`, `src/scene.c`, `src/math_utils.c`, `src/lighting.c`, `src/materials.c`, `src/textures.c`, their headers, the Makefile, bundled assets, and the README as inspected on this document's date. These source files are the authority for implemented features.

Technology recommendations were checked against the official Three.js and Vite documentation linked above. Product scope, repository layout, performance budgets, and migration phases are proposed engineering decisions, not claims that those changes already exist.
