# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

A classic JRPG built with C++ and Raylib. The project follows a phased development roadmap (see ROADMAP.md). Phase 1 (Foundation) and Phase 2 (Core RPG Mechanics) are complete with character stats, party management, state machine, and scene management systems in place.

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

#### Phase 1 Systems (Foundation)
- **Tilemap** (`tilemap.h/cpp`) - Stores and renders tile-based maps as 2D arrays. Each tile ID maps to a visual representation (grass=0, wall=1, water=2, path=3). Provides collision detection via `isWalkable()`.

- **Player** (`player.h/cpp`) - Player character with both tile coordinates and pixel coordinates. Uses smooth interpolated movement between tiles at 4 tiles/second. Handles input via WASD/arrow keys and performs collision checks before moving.

- **Sprite** (`sprite.h/cpp`) - Handles sprite loading and animated rendering. Supports frame-based animation with configurable frame counts and animation speeds.

- **Camera** (`camera.h/cpp`) - Camera system that follows the player while respecting map boundaries. Provides offsets for rendering all game elements.

#### Phase 2 Systems (Core RPG Mechanics)
- **CharacterStats** (`character_stats.h/cpp`) - RPG stat management with HP, MP, Attack, Defense, Level, and Experience. Includes damage calculation, healing, MP usage, and level-up mechanics with stat growth formulas.

- **PartyMember** (`party_member.h/cpp`) - Represents individual party members with a name, character class (Warrior, Mage, Cleric, Thief), and CharacterStats. Supports sprite/portrait paths for future visual representation.

- **Party** (`party.h/cpp`) - Manages the player's party with up to 4 active members and 8 reserve members. Provides party-wide operations like healing all, restoring MP, and gaining experience. Includes member swapping and reserve management.

- **StateManager** (`state_manager.h/cpp`) - Game state machine managing four states: EXPLORATION, BATTLE, MENU, and DIALOG. Supports state transitions with enter/exit callbacks for each state.

- **SceneManager** (`scene_manager.h/cpp`) - Scene management system with factory-based scene registration. Handles scene transitions with proper lifecycle management (onEnter/onExit).

- **Scene** (`scene.h`) - Abstract base class for all game scenes. Defines the interface for scene lifecycle and main loop methods.

- **ExplorationScene** (`exploration_scene.h/cpp`) - Concrete scene implementation for exploration gameplay. Encapsulates the tilemap, player, and camera systems from Phase 1.

### Key Coordinate System
The game uses dual coordinate systems:
- **Tile coordinates** - Logical grid position (m_tileX, m_tileY)
- **Pixel coordinates** - Screen position for rendering (m_pixelX, m_pixelY)

Movement interpolates between tile positions for smooth animation. All rendering functions accept camera offsets to implement scrolling.

### Development Phases
The project follows ROADMAP.md:
- **Phase 1 (Foundation)** ✅ - Tilemap rendering, sprite system, player movement, collision detection, and camera following
- **Phase 2 (Core RPG Mechanics)** ✅ - Character stats, party management, state machine, and scene management
- **Phase 3 (Battle System)** - Next: Turn-based battle, enemy system, combat mechanics

## Code Patterns

- All game systems use unique_ptr ownership by the Game class
- The Game class now uses StateManager for game states and SceneManager for scene transitions
- Scenes are registered via factory functions in `Game::initializeGame()`
- Map data is defined in `ExplorationScene::initializeMap()` for the exploration scene
- The Party system is initialized with a default "Hero" character in `Game::initializeParty()`
- Tile size is 32px, map is 30x20 tiles, window is 800x600px
- State-specific update/draw logic is delegated based on the current GameState
