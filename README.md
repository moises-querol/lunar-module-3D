# Lunar Module 3D

An interactive model of the Apollo 11 Eagle lunar module, available as a TypeScript/Three.js browser viewer and the original C/OpenGL desktop application. The project began as a computer graphics coursework project in 2007.

Inspect the faceted cabin, gold octagonal base, landing legs, footpads, ladder, ramp, and flag. Choose Moon, Mars, or Venus terrain and a USA, Spain, or USSR flag independently. These are visual variations of an educational model, not a flight simulation or a historically exact reconstruction.

## Browser version

The browser viewer lives in `html/` and uses TypeScript, Three.js, and Vite. It preserves the native model's geometry and color palette, with modern metallic materials, camera-relative inspection lighting, shadows, and panel outlines. It includes mouse, keyboard, and touch controls, reset view, and loading/error messages with retry support.

Both versions reference the same original `assets/textures/` files. The browser loads TGA files directly, without maintaining converted images or another source texture collection. It runs as a static application without a backend or database.

### Run locally

Use Node.js 22.12 or newer and a browser with WebGL 2:

```sh
cd html
npm ci
npm run dev
```

Open the address printed by Vite, normally `http://127.0.0.1:5173/`. Keep the repository's shared `assets/` directory alongside `html/` when developing from source.

### Browser controls

| Action | Mouse / interface | Keyboard while the model is focused |
| --- | --- | --- |
| Orbit | Left-drag | Arrow keys |
| Zoom | Scroll or middle-drag | + / − |
| Pan | Right-drag | Shift + arrow keys |
| Reset camera | Reset view button | R |
| Show controls | ? button | H |
| Close controls | Close button | Escape |
| Change location or flag | Labeled selectors | Tab to the selector and use arrow keys |

On touch screens, use one finger to orbit and two fingers to zoom or pan. Reset view restores the camera without changing the selected location or flag. The browser camera stays upright, unlike the native application's free trackball rotation.

### Production build

To create and preview the static production build:

```sh
npm run build
npm run preview
```

Run these commands from `html/`. The build checks TypeScript and produces a self-contained `html/dist/` folder. The preview command serves that build locally; it is not a production server.

Deploy the contents of `html/dist/` with a static HTTP/HTTPS host. Vite bundles the shared textures into this ignored, generated output, so deployment does not require the native source tree. Opening the source HTML directly from the filesystem is not supported.

### Updating the cabin geometry

The cabin's 140 flattened panels are checked in at `html/src/cabin-data.json`. If the native cabin changes, regenerate this data from inside `html/`:

```sh
npm run import-model
```

The importer reads `src/scene.c` and preserves the cabin's coordinates and transforms while orienting its faces outward. Ordinary development and production builds use the checked-in data and do not require this command. Other procedural parts are implemented in `html/src/model.ts`.

## Native version requirements

A C compiler, GNU Make, OpenGL, GLU, and GLUT/freeglut development libraries, plus a graphical desktop session.

On Debian or Ubuntu:

```sh
sudo apt install build-essential pkg-config freeglut3-dev libgl1-mesa-dev libglu1-mesa-dev
```

On Fedora:

```sh
sudo dnf install gcc make pkgconf-pkg-config freeglut-devel mesa-libGL-devel mesa-libGLU-devel
```

The Makefile also includes macOS build paths for Homebrew freeglut or Apple's OpenGL/GLUT frameworks. macOS compatibility has not been verified.

## Native build and run

Run these commands from the repository root; texture paths are relative to that directory.

```sh
make
./lunar-module
```

`make` creates the executable and stores intermediate object and dependency files in `build/`. Generated files are ignored by Git. To remove them:

```sh
make clean
```

## Native controls

| Action | Control |
| --- | --- |
| Rotate | Left-drag |
| Zoom | Middle-drag |
| Pan | Shift + middle-drag |
| Select terrain or flag | Right-click menu |
| Quit | Escape |

## Project layout

```text
src/                  Native C application and scene geometry
html/src/             TypeScript viewer, model, materials, controls, and interface
html/scripts/         Cabin geometry importer
html/package.json     Browser dependencies and development/build commands
html/dist/            Generated browser production build, ignored by Git
assets/textures/      Shared terrain and flag textures
assets/images/        Original native screenshots
Makefile              Native build configuration
DESIGN.md             Product design, architecture, and modernization rationale
```

## Rendering and assets

The browser renderer uses Three.js with WebGL 2, explicit triangle meshes, physically based materials, and a shadow-casting key light. Terrain and flags reuse the original TGA assets with their native texture orientation. Rendering stops when the view is idle, and the interface adapts to desktop and mobile screens.

The native renderer uses legacy OpenGL immediate mode and display lists and requires a compatibility context. Its geometry is defined directly in C. Its texture loader supports the included 256 × 256, uncompressed 24-bit TGA files with a bottom-left origin.

See [DESIGN.md](DESIGN.md) for the feature inventory, architecture, and technology choices.

## Native screenshots

These images show the original desktop application; the browser version adds its own control panel, modern lighting, and shadows.

![Lunar module on Mars with the USA flag](assets/images/mars-usa.png)

![Lunar module on Venus with the Spain flag](assets/images/venus-spain.png)
