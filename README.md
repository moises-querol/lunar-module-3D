# Lunar Module 3D

An interactive C and OpenGL model of the Apollo 11 Eagle lunar module, originally created as a computer graphics coursework project (2007). Explore the model with mouse controls and choose between Moon, Mars, and Venus terrain and USA, Spain, and USSR flags.

## Requirements

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

## Build and run

Run these commands from the repository root; texture paths are relative to that directory.

```sh
make
./lunar-module
```

`make` creates the executable and stores intermediate object and dependency files in `build/`. Generated files are ignored by Git. To remove them:

```sh
make clean
```

## Controls

| Action | Control |
| --- | --- |
| Rotate | Left-drag |
| Zoom | Middle-drag |
| Pan | Shift + middle-drag |
| Select terrain or flag | Right-click menu |
| Quit | Escape |

## Project layout

```text
src/             Application, scene geometry, lighting, materials, textures, and math
assets/textures/ Terrain and flag textures
assets/images/   Example screenshots
Makefile         Build configuration
```

## Rendering and assets

The renderer uses legacy OpenGL immediate mode and display lists and requires a compatibility context. Geometry is defined directly in C. Texture loading supports the included 256 × 256, uncompressed 24-bit TGA files with a bottom-left origin.

## Screenshots

![Lunar module on Mars with the USA flag](assets/images/mars-usa.png)

![Lunar module on Venus with the Spain flag](assets/images/venus-spain.png)
