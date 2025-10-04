# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

A classic JRPG built with C++ and Raylib. The project follows a phased development roadmap (see ROADMAP.md). Phase 1 (Foundation), Phase 2 (Core RPG Mechanics), and Phase 3 (Battle System) are complete.

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

- **SceneManager** (`scene_manager.h/cpp`) - Unified state and scene management using GameState enum (EXPLORATION, BATTLE, MENU, DIALOG). Scenes are created once and kept alive (never destroyed) to avoid memory issues. Direct enum-based transitions via `changeState()`.

- **Scene** (`scene.h`) - Abstract base class for all game scenes. Defines the interface for scene lifecycle (onEnter/onExit) and main loop methods (update/draw).

- **ExplorationScene** (`exploration_scene.h/cpp`) - Concrete scene implementation for exploration gameplay. Encapsulates the tilemap, player, and camera systems from Phase 1. Press 'B' to trigger battle transitions.

#### Phase 3 Systems (Battle System)
- **Enemy** (`enemy.h/cpp`) - Enemy entities with CharacterStats, AI behavior types (Aggressive, Balanced, Defensive, Support), and reward calculations (EXP/gold based on level).

- **EnemyFormation** (`enemy_formation.h/cpp`) - Manages groups of enemies in battle. Tracks alive count, handles removal of defeated enemies, and calculates total rewards.

- **BattleScene** (`battle_scene.h/cpp`) - Turn-based battle implementation with:
  - Speed-based initiative system (uses Attack stat + random for turn order)
  - Battle states (Turn Start, Player/Enemy Select, Action Execution, Victory/Defeat/Fled)
  - Player command menu (Attack, Magic, Item, Defend, Run)
  - Combat math: damage = attacker.attack - defender.defense/2, 90% hit rate, 10% crit rate
  - Simple enemy AI that targets random alive party members
  - Victory awards EXP to all party members
  - Defeat/flee transitions back to exploration

### Key Coordinate System
The game uses dual coordinate systems:
- **Tile coordinates** - Logical grid position (m_tileX, m_tileY)
- **Pixel coordinates** - Screen position for rendering (m_pixelX, m_pixelY)

Movement interpolates between tile positions for smooth animation. All rendering functions accept camera offsets to implement scrolling.

### Development Phases
The project follows ROADMAP.md:
- **Phase 1 (Foundation)** ✅ - Tilemap rendering, sprite system, player movement, collision detection, and camera following
- **Phase 2 (Core RPG Mechanics)** ✅ - Character stats, party management, unified state/scene management
- **Phase 3 (Battle System)** ✅ - Turn-based battles, enemy system, combat mechanics, battle UI
- **Phase 4 (Progression Systems)** - Next: Inventory, equipment, magic/skills

## Code Patterns

- All game systems use unique_ptr ownership by the Game class
- **Scenes are created once and kept alive** - No destruction/recreation on transitions
- Scenes registered via `SceneManager::registerScene(GameState, unique_ptr<Scene>)` in `Game::initializeGame()`
- State transitions use `SceneManager::changeState(GameState)` - single call, enum-based
- Access other scenes via `SceneManager::getScene(GameState)` - safe, always valid
- Map data is defined in `ExplorationScene::initializeMap()` for the exploration scene
- The Party system is initialized with a default "Hero" character in `Game::initializeParty()`
- Tile size is 32px, map is 30x20 tiles, window is 800x600px
- Game::update/draw simply delegate to SceneManager - no state checking needed

## Important Design Decisions

### Scene Management Architecture
- **Scenes stay alive for the entire game** - They are created once during initialization and never destroyed
- This eliminates use-after-free bugs from self-destruction during scene transitions
- Callbacks can safely capture SceneManager pointer (owned by Game, always valid)
- No static members or factory functions needed - direct unique_ptr registration
- GameState enum is the single source of truth for both state and scene identity

### Battle System
- Battles use the existing BattleScene instance - configure it before transition via `setEnemyFormation()` and `setOnBattleEndCallback()`
- Press 'B' in exploration to trigger a test battle (Slime + Goblin)
- Turn order recalculated each round based on initiative
- Combat is functional but Magic and Item commands are not yet implemented
