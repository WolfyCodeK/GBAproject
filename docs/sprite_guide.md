# Sprite Organization Guide

Your GBA project now has a clean, organized structure for managing sprites and colors:

## File Structure

```
├── main.c          # Main game logic
├── gba.h           # GBA hardware definitions  
├── sprites.h       # All sprite definitions
├── colors.h        # All color palettes
└── png_to_sprite.py # Automatic PNG converter
```

## How It Works

### 1. Sprites (`sprites.h`)
- Contains all sprite array definitions
- Each sprite is a 2D array of palette indices
- Automatically updated by the PNG converter

### 2. Colors (`colors.h`) 
- Contains all color palette definitions
- Uses GBA's RGB(r, g, b) format (0-31 per channel)
- Automatically updated by the PNG converter

### 3. Main Game (`main.c`)
- Includes both header files
- Uses sprites with: `drawSprite(x, y, spriteName, width, height, colorPalette)`

## Adding New Sprites

### Method 1: PNG Converter (Recommended)
```bash
python png_to_sprite.py myimage.png mySprite 5
```
This automatically:
- ✅ Converts PNG to sprite array
- ✅ Extracts color palette  
- ✅ Adds to sprites.h and colors.h
- ✅ Shows usage code

### Method 2: Manual Addition
1. Add sprite array to `sprites.h`
2. Add color palette to `colors.h`
3. Use in game with `drawSprite()`

## Current Sprites Available

- `playerSprite` (8x8) - Player character
- `dirt` (16x16) - Dirt texture
- `digitFont` (3x5) - Number font for UI

## Current Color Palettes

- `playerColors` - For player character
- `dirtColors` - For dirt texture
- `gameColors` - General purpose colors
- `worldColors` - For world rendering

## Example Usage

```c
// In main.c game loop:
drawSprite(P.x-4, P.y-4, playerSprite, 8, 8, playerColors);
drawSprite(20, 20, dirt, 16, 16, dirtColors);
```

## Benefits

- 🧹 **Clean main.c** - No clutter from sprite data
- 📁 **Organized** - Sprites and colors in separate files
- 🔄 **Automatic** - PNG converter handles everything
- 🎨 **Scalable** - Easy to add many sprites
- 📝 **Maintainable** - Clear separation of concerns 