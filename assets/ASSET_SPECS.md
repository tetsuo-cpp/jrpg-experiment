# Asset Specifications for JRPG Game

This document describes the sprite and tileset assets needed to replace the placeholder graphics.

## Directory Structure

All assets should be placed in the `/assets/` directory with the following file names:

```
assets/
├── tileset.png      # Tileset for map tiles
├── player.png       # Player character sprite sheet (animated)
├── villager.png     # Villager NPC sprite (static)
├── guard.png        # Guard NPC sprite (static)
└── merchant.png     # Merchant NPC sprite (static)
```

## Tileset (tileset.png)

**Format:** PNG with transparency support
**Tile Size:** 32×32 pixels per tile
**Layout:** Grid arrangement with 8 tiles per row
**Recommended Image Size:** 256×256 pixels (8×8 tiles)

### Required Tiles:

The tileset must include at least these tiles in the following IDs:

- **Tile 0:** Grass (walkable)
- **Tile 1:** Wall (not walkable)
- **Tile 2:** Water (not walkable)
- **Tile 3:** Dirt/Path (walkable)

**Tile ID Calculation:**
```
TileID = (row * 8) + column
```

For example:
- Tile at position (0,0) = ID 0
- Tile at position (1,0) = ID 1
- Tile at position (2,0) = ID 2
- Tile at position (3,0) = ID 3

You can add additional tiles beyond ID 3 for future use.

## Player Sprite Sheet

The player character uses an animated sprite sheet with walk cycles.

**Format:** PNG with transparency support
**Frame Size:** 32×32 pixels per frame
**Total Image Size:** 64×128 pixels
**Layout:** 2 columns (animation frames) × 4 rows (directions)

### Sprite Sheet Layout:

```
┌─────────┬─────────┐
│ Down-0  │ Down-1  │  Row 0: Down-facing
├─────────┼─────────┤
│ Left-0  │ Left-1  │  Row 1: Left-facing
├─────────┼─────────┤
│ Right-0 │ Right-1 │  Row 2: Right-facing
├─────────┼─────────┤
│ Up-0    │ Up-1    │  Row 3: Up-facing
└─────────┴─────────┘
   32px      32px

Each row = 64px
Each column = 32px
```

### Row Assignments:
- **Row 0 (pixels 0-31):** Down-facing walk cycle (2 frames)
- **Row 1 (pixels 32-63):** Left-facing walk cycle (2 frames)
- **Row 2 (pixels 64-95):** Right-facing walk cycle (2 frames)
- **Row 3 (pixels 96-127):** Up-facing walk cycle (2 frames)

### Animation Details:
- **Frame 0:** Idle/standing pose
- **Frame 1:** Walking animation (alternates with frame 0)
- **Animation Speed:** 0.15 seconds per frame
- The game automatically cycles through both frames when the character is moving
- Frame 0 is displayed when the character is standing still

## NPC Sprites

NPCs are stationary and don't animate, so they only need single-frame sprites.

**Format:** PNG with transparency support
**Image Size:** 32×32 pixels (single frame)
**No animation needed**

Since NPCs don't move, you just need one simple 32×32 pixel image per NPC showing them facing forward/down.

## Required Assets

### 1. player.png
**Type:** Animated sprite sheet
**Size:** 64×128 pixels (2×4 frames)
**Description:** Main player character sprite with walk animations
**Suggested Style:** Heroic character with distinct appearance

### 2. villager.png
**Type:** Static sprite
**Size:** 32×32 pixels
**Description:** Generic villager NPC
**Suggested Style:** Friendly civilian appearance
**Note:** Triggers dialog interaction

### 3. guard.png
**Type:** Static sprite
**Size:** 32×32 pixels
**Description:** Guard NPC
**Suggested Style:** Armored or uniformed guard
**Note:** Triggers dialog interaction

### 4. merchant.png
**Type:** Static sprite
**Size:** 32×32 pixels
**Description:** Merchant/shop keeper NPC
**Suggested Style:** Shopkeeper with distinctive merchant appearance
**Note:** Triggers shop interaction

## Fallback Behavior

If asset files are not found or fail to load, the game will automatically fall back to colored placeholder graphics:

- **Player:** Yellow rectangle
- **NPCs (Dialog):** Blue rectangle
- **NPCs (Shop):** Orange rectangle
- **Tiles:** Colored rectangles (Green=Grass, Gray=Wall, Blue=Water, Brown=Path)

This allows you to add sprites incrementally—you don't need all assets at once.

## Testing Your Assets

1. Place your PNG files in the `/assets/` directory
2. Build and run the game: `cmake --build build && ./build/jrpg_game`
3. The game will automatically attempt to load the assets
4. Check the console output for any loading errors
5. If a file fails to load, the placeholder graphics will be used instead

## Image Creation Tips

- Use transparent backgrounds (PNG alpha channel)
- Keep pixel art crisp—avoid anti-aliasing on edges
- Ensure each frame is centered within its 32×32 cell
- For walk cycles, the 2-frame animation alternates between frame 0 and frame 1
- Test with simple sprites first before creating detailed artwork
- GIMP, Aseprite, or Photoshop work well for creating these sprite sheets

## Example Workflow

1. **Create a tileset:**
   - Make a 256×256 pixel canvas
   - Divide it into 8×8 grid of 32×32 tiles
   - Draw grass, wall, water, and path tiles in positions 0-3

2. **Create the player sprite sheet:**
   - Make a 64×128 pixel canvas
   - Divide it into 2×4 grid of 32×32 frames
   - Draw down-facing frames in row 0 (2 frames)
   - Draw left-facing frames in row 1 (2 frames)
   - Draw right-facing frames in row 2 (2 frames)
   - Draw up-facing frames in row 3 (2 frames)

3. **Create NPC sprites:**
   - Make a 32×32 pixel canvas for each NPC
   - Draw a single front-facing or down-facing pose
   - No animation needed—just one frame per NPC

4. **Export and test:**
   - Export as PNG with transparency
   - Place in `/assets/` directory
   - Run the game to see your sprites in action
