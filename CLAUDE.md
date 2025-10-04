# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

A classic JRPG built with C++ and Raylib. The project follows a phased development roadmap (see ROADMAP.md). Phase 1 (Foundation), Phase 2 (Core RPG Mechanics), Phase 3 (Battle System), Phase 4 (Progression Systems), and Phase 5 (UI & Interaction) are complete.

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

- **Party** (`party.h/cpp`) - Manages the player's party with up to 4 active members and 8 reserve members. Provides party-wide operations like healing all, restoring MP, and gaining experience. Includes member swapping and reserve management. Gold tracking system with starting balance of 500 gold.

- **SceneManager** (`scene_manager.h/cpp`) - Unified state and scene management using GameState enum (EXPLORATION, BATTLE, MENU, DIALOG, SHOP). Scenes are created once and kept alive (never destroyed) to avoid memory issues. Direct enum-based transitions via `changeState()`.

- **Scene** (`scene.h`) - Abstract base class for all game scenes. Defines the interface for scene lifecycle (onEnter/onExit) and main loop methods (update/draw).

- **ExplorationScene** (`exploration_scene.h/cpp`) - Concrete scene implementation for exploration gameplay. Encapsulates the tilemap, player, and camera systems from Phase 1. Press 'B' to trigger battle transitions.

#### Phase 3 Systems (Battle System)
- **Enemy** (`enemy.h/cpp`) - Enemy entities with CharacterStats, AI behavior types (Aggressive, Balanced, Defensive, Support), and reward calculations (EXP/gold based on level).

- **EnemyFormation** (`enemy_formation.h/cpp`) - Manages groups of enemies in battle. Tracks alive count, handles removal of defeated enemies, and calculates total rewards.

- **BattleScene** (`battle_scene.h/cpp`) - Turn-based battle implementation with:
  - Speed-based initiative system (uses Attack stat + random for turn order)
  - Battle states (Turn Start, Player/Enemy/Skill/Item/Target Select, Action Execution, Victory/Defeat/Fled)
  - Player command menu (Attack, Magic, Item, Defend, Run) - all commands fully functional
  - Combat math: damage = attacker.attack - defender.defense/2, 90% hit rate, 10% crit rate
  - Magic system with skill selection, MP cost checking, offensive/healing spells
  - Item system with inventory integration and consumption
  - Simple enemy AI that targets random alive party members
  - Victory awards EXP to all party members
  - Defeat/flee transitions back to exploration

#### Phase 4 Systems (Progression Systems)
- **Item** (`item.h/cpp`) - Item class with types (Consumable, KeyItem, Material, Equipment) and effects (Restore HP/MP, Revive). Includes battle/field usability checks (`isUsableInBattle()`, `isUsableInField()`) and buy/sell prices.

- **Inventory** (`inventory.h/cpp`) - Party-wide shared inventory with 64 slots using vector-based storage. Uses Item* pointers to support polymorphism (Item and Equipment). Stack management (max 99 per item), add/remove with quantity tracking. Dynamic memory management with proper cleanup in destructor.

- **Equipment** (`equipment.h/cpp`) - Inherits from Item. Equipment types (Weapon, Armor, Accessory) with stat bonuses (Attack, Defense, HP, MP). Accessed via `getEquipmentType()` to distinguish from base Item type. Buy/sell price support for shop system.

- **Skill** (`skill.h/cpp`) - Skill system with types (Offensive Magic, Healing Magic, Buff, Debuff) and targets (Single Enemy, All Enemies, Single Ally, All Allies, Self). MP cost and power values. Class-specific skill learning.

- **PartyMember** (extended) - Now includes equipment slots (weapon, armor, accessory) with automatic stat recalculation on equip/unequip. Provides both unique_ptr-based and raw pointer equipment methods. Added `getClassName()` helper for UI display. Skill learning and management system.

- **CharacterStats** (extended) - Equipment bonus system with base stats + equipment modifiers. Separate accessors for base and modified stats. HP/MP accessed via `getHP()`/`getMP()`, experience via `getExperience()`/`getExperienceToNextLevel()`.

### Key Coordinate System
The game uses dual coordinate systems:
- **Tile coordinates** - Logical grid position (m_tileX, m_tileY)
- **Pixel coordinates** - Screen position for rendering (m_pixelX, m_pixelY)

Movement interpolates between tile positions for smooth animation. All rendering functions accept camera offsets to implement scrolling.

#### Phase 5 Systems (UI & Interaction)
- **MenuScene** (`menu_scene.h/cpp`) - Comprehensive menu system accessible via ESC/M from exploration. Implements:
  - **Main Menu**: Navigation between Status, Items, Equipment, and Save submenus
  - **Status Submenu**: View party member stats (name, class, level, EXP, HP/MP, Attack/Defense, equipped items). Navigate between members with arrow keys.
  - **Items Submenu**: Browse inventory with scrolling (15 items per page). Use consumables on party members for HP/MP restoration. Shows "[Battle only]" indicator for non-field items. Automatic quantity updates and item removal.
  - **Equipment Submenu**: Three-level navigation (select member → select slot → choose equipment). Equip/unequip weapons, armor, accessories. Shows stat bonuses (ATK+, DEF+, HP+, MP+). Press X to unequip current item. Handles equipment transfer between inventory and party members with proper ownership management.
  - **Save Submenu**: Placeholder for Phase 6 implementation
  - Uses callback system to return to exploration scene

- **DialogScene** (`dialog_scene.h/cpp`) - Dialog system for NPC conversations:
  - Text box UI with speaker names and text wrapping
  - Multi-line dialog sequences with SPACE/ENTER to advance
  - Choice system with arrow key navigation for dialog branching
  - Dialog registration by ID for NPC triggers
  - ESC to close dialog (when no choices active)
  - Callback system to return to previous scene

- **NPC** (`npc.h/cpp`) - Non-player character system:
  - NPCType enum (DIALOG or SHOP) to determine interaction behavior
  - Position tracking (tile and pixel coordinates)
  - Adjacent detection for player interaction (1 tile away in cardinal directions)
  - Visual distinction (blue for dialog NPCs, orange for shop NPCs)
  - Dialog ID for triggering specific conversations
  - Interaction via SPACE/ENTER when adjacent

- **Shop** (`shop.h/cpp`) - Shop inventory and transaction management:
  - Shop-owned item copies with unlimited (-1) or limited stock
  - Support for both consumables and equipment
  - Stock tracking and depletion on purchase
  - Buy/sell price integration with Item system

- **ShopScene** (`shop_scene.h/cpp`) - Shop interface for buying/selling:
  - **Main Menu**: Buy, Sell, Leave options
  - **Buy Screen**: Scrollable shop inventory (12 items per page), price display, stock indicators (∞ or quantity), gold check before purchase
  - **Sell Screen**: Scrollable player inventory, sell price display, quantity tracking
  - **Confirmation Screens**: Quantity adjustment (left/right arrows), total cost/value display, affordability check
  - Gold display in top-right corner
  - Automatic inventory updates on transactions
  - Callback system to return to exploration

- **Party** (extended) - Added `getActiveMembers()` methods that return vectors of PartyMember pointers for UI iteration. Gold management with `getGold()`, `addGold()`, `spendGold()` methods.

- **Inventory** (extended) - Added `removeItem(int slotIndex, int quantity)` overload for shop selling by index

### Development Phases
The project follows ROADMAP.md:
- **Phase 1 (Foundation)** ✅ - Tilemap rendering, sprite system, player movement, collision detection, and camera following
- **Phase 2 (Core RPG Mechanics)** ✅ - Character stats, party management, unified state/scene management
- **Phase 3 (Battle System)** ✅ - Turn-based battles, enemy system, combat mechanics, battle UI
- **Phase 4 (Progression Systems)** ✅ - Inventory, equipment, magic/skills, leveling with stat growth
- **Phase 5 (UI & Interaction)** ✅ - Menu system, dialog system, NPC system, shop system
- **Phase 6 (Persistence & Polish)** - Next: Save/Load, sound effects, music, transitions

## Code Patterns

- All game systems use unique_ptr ownership by the Game class
- **Scenes are created once and kept alive** - No destruction/recreation on transitions
- Scenes registered via `SceneManager::registerScene(GameState, unique_ptr<Scene>)` in `Game::initializeGame()`
- State transitions use `SceneManager::changeState(GameState)` - single call, enum-based
- Access other scenes via `SceneManager::getScene(GameState)` - safe, always valid
- Map data is defined in `ExplorationScene::initializeMap()` for the exploration scene
- The Party system is initialized with 3 test characters (Hero/Warrior, Mage, Cleric) and 500 gold in `Game::initializeParty()`
- Inventory is initialized with test items (consumables: Potions, Hi-Potions, Ethers, Elixirs; equipment: Iron Sword, Steel Sword, Magic Staff, Leather Armor, Chain Mail, Mage Robe, Power Ring, HP Ring) in `Game::initializeInventory()`
- Shop is initialized with various items and equipment (potions, weapons, armor, accessories) with mixed unlimited and limited stock in `Game::initializeShop()`
- NPCs are initialized in `ExplorationScene::initializeNPCs()` - Villager (10,8), Guard (18,12), Merchant (7,14)
- Dialogs are registered in `Game::initializeGame()` with IDs 1-5 for different NPC conversations
- Characters learn class-specific skills on initialization
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
- BattleScene requires both Party* and Inventory* pointers (passed from Game class)
- Press 'B' in exploration to trigger a test battle (Slime + Goblin)
- Turn order recalculated each round based on initiative
- All 5 battle commands are fully functional:
  - **Attack** - Physical damage to single enemy
  - **Magic** - Skill selection menu → target selection → MP consumption and effect (damage or healing)
  - **Item** - Item selection menu (battle-usable only) → target selection → item consumption and effect
  - **Defend** - Placeholder (not yet implemented)
  - **Run** - 50% flee chance
- UI shows HP/MP for party, navigable with ESC/Backspace to cancel selections
- Skills show MP cost and gray out when insufficient MP
- Items show quantity in inventory

### Menu System
- MenuScene is created and registered in `Game::initializeGame()` with callback to return to exploration
- Press ESC or M in exploration to open menu
- **IMPORTANT**: `SetExitKey(KEY_NULL)` is called in Game constructor to disable ESC-to-close-window behavior, allowing ESC to be used for menu navigation
- All menu navigation uses arrow keys, ENTER/SPACE to confirm, ESC/BACKSPACE to go back
- Equipment management transfers ownership between inventory (Item* stored in vector) and party members (unique_ptr in PartyMember)
- When unequipping, creates a copy of Equipment and adds to inventory, then releases the unique_ptr
- When equipping, removes from inventory (doesn't delete), then transfers raw pointer to party member via `reset()`
- Item usage in field properly handles HP/MP restoration and item consumption via Inventory::removeItem()
- Window can only be closed via the OS window close button (not ESC)

### Dialog System
- DialogScene is created and registered in `Game::initializeGame()` with callback to return to exploration
- Dialog instances are registered with unique IDs (1-5 for test NPCs)
- Dialog supports multi-line sequences, speaker names, and choice branching
- Choice system allows jumping to different dialog IDs or ending conversation (-1)
- Press SPACE/ENTER to advance dialog, arrow keys to select choices
- ESC closes dialog (only when not showing choices)

### NPC & Shop System
- NPCs are placed on the tilemap with NPCType (DIALOG or SHOP)
- Press SPACE/ENTER when adjacent to NPC to interact
- DIALOG NPCs trigger DialogScene with their dialog ID
- SHOP NPCs trigger ShopScene directly
- Shop owned by Game class, initialized with items in `initializeShop()`
- ShopScene handles all buy/sell transactions with gold and inventory integration
- Visual distinction: Blue NPCs = dialog, Orange NPCs = shop
- Current NPCs: Villager (dialog), Guard (dialog with choices), Merchant (shop)
