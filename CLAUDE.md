# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

A classic JRPG built with C++ and Raylib. The project follows a phased development roadmap (see ROADMAP.md), currently in Phase 1 with movement and world exploration complete.

## Build Commands

```bash
# Build the project (from project root)
cmake -B build
cmake --build build

# Run the game
./build/jrpg_game
```

The build system uses CMake and automatically fetches Raylib 5.0 as a dependency.

## Project Architecture

### Core Game Loop
- `main.cpp` - Entry point, instantiates and runs the Game
- `game.h/cpp` - Main game class that orchestrates all systems via `update()` and `draw()` methods at 60 FPS

### Component Systems
The game uses a modular component architecture where each system is owned by the Game class:

- **Tilemap** (`tilemap.h/cpp`) - Stores and renders tile-based maps as 2D arrays. Each tile ID maps to a visual representation (grass=0, wall=1, water=2, path=3). Provides collision detection via `isWalkable()`.

- **Player** (`player.h/cpp`) - Player character with both tile coordinates and pixel coordinates. Uses smooth interpolated movement between tiles at 4 tiles/second. Handles input via WASD/arrow keys and performs collision checks before moving.

- **Sprite** (`sprite.h/cpp`) - Handles sprite loading and animated rendering. Supports frame-based animation with configurable frame counts and animation speeds.

- **Camera** (`camera.h/cpp`) - Camera system that follows the player while respecting map boundaries. Provides offsets for rendering all game elements.

### Key Coordinate System
The game uses dual coordinate systems:
- **Tile coordinates** - Logical grid position (m_tileX, m_tileY)
- **Pixel coordinates** - Screen position for rendering (m_pixelX, m_pixelY)

Movement interpolates between tile positions for smooth animation. All rendering functions accept camera offsets to implement scrolling.

### Development Phases
The project follows ROADMAP.md. Phase 1 (Foundation) is complete with tilemap rendering, sprite system, player movement, collision detection, and camera following. Next phases will add character stats, party management, turn-based battle system, and progression mechanics.

## Code Patterns

- All game systems use unique_ptr ownership by the Game class
- Map data is hardcoded in `Game::initializeMap()` - modify this for different map layouts
- Tile size is 32px, map is 30x20 tiles, window is 800x600px
- The Game class owns instances of Tilemap, Player, and GameCamera, passing references between them as needed (e.g., Player needs Tilemap reference for collision detection)
