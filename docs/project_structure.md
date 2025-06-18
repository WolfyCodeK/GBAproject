# GBA Project Structure

Your Game Boy Advance project is now organized with a professional folder structure:

## 📁 Directory Structure

```
GBAproject/
├── 📂 src/           # Source code
│   ├── main.c        # Main game logic
│   ├── sprites.h     # Sprite declarations
│   ├── colors.h      # Color palette declarations
│   └── gba.h         # GBA hardware definitions
│
├── 📂 res/           # Resources
│   └── *.png         # Sprite images
│
├── 📂 scripts/       # Build tools & utilities
│   ├── png_to_sprite.py  # PNG to sprite converter
│   ├── requirements.txt  # Python dependencies
│   └── make.bat          # Old build script (deprecated)
│
├── 📂 docs/          # Documentation
│   ├── README.md          # Project overview
│   ├── project_structure.md  # This file
│   ├── sprite_guide.md    # Sprite creation guide
│   └── CONTRIBUTING.md    # Contribution guidelines
│
├── 📂 tools/         # Development tools
│   └── VisualBoyAdvance.exe  # GBA Emulator
│
├── 📂 lib/           # Libraries & dependencies
│   └── devkitadv/    # DevkitAdvance toolchain
│
├── 📂 bin/           # Compiled binaries
│   └── main.gba      # Compiled game (created during build)
│
├── 🔨 build.bat      # Main build script
├── ⚙️ vba.ini        # Emulator configuration (auto-generated)
├── LICENSE           # License file
└── COPYING           # Copyright information
```

## 🚀 Quick Start

### Building the Project
```bash
# From project root:
./build.bat
```

### Adding New Sprites
```bash
# From scripts/ directory:
cd scripts
python png_to_sprite.py ../res/myimage.png mySprite 5
```

### Running the Game
```bash
# Automatic after build, or manually:
tools/VisualBoyAdvance.exe bin/main.gba
```

## 📝 Development Workflow

### 1. **Source Code** (`src/`)
- Edit `main.c` for game logic
- Sprite and color definitions are automatically added by the converter
- Headers contain `extern` declarations only

### 2. **Resources** (`res/`)
- Store PNG images for sprites
- Organize by category (sprites/, sounds/, etc.)

### 3. **Scripts** (`scripts/`)
- Use `png_to_sprite.py` to convert artwork
- Build tools and automation scripts
- Run from this directory: `cd scripts && python png_to_sprite.py ...`

### 4. **Documentation** (`docs/`)
- Project documentation and guides
- Keep README and guides up to date
- Add new documentation as project grows

### 5. **Build Output** (`bin/`)
- Contains compiled `.gba` files
- Temporary build files are cleaned automatically
- Ready to run on emulator or hardware

### 6. **Configuration Files**
- `vba.ini` - Emulator settings (stays in root, auto-updated by emulator)
- Contains window position, ROM directory, and display settings

## 🎯 Benefits of This Structure

- **🧹 Clean separation** - Code, resources, tools, and docs are separate
- **📈 Scalable** - Easy to add more sprites, sounds, levels
- **🔧 Professional** - Industry-standard project organization
- **🤝 Collaborative** - Easy for multiple developers to work together
- **📦 Portable** - Self-contained with all dependencies

## 🔄 Migration Notes

### What Changed:
- All source files moved to `src/`
- Build script updated for new paths
- PNG converter updated for new structure
- Documentation organized in `docs/`
- `vba.ini` remains in root for emulator compatibility

### Backward Compatibility:
- Old `make.bat` still available in `scripts/` (deprecated)
- New `build.bat` handles all paths automatically
- All functionality preserved

## 📚 Related Documentation

- [Sprite Creation Guide](sprite_guide.md) - How to create and add sprites
- [README](README.md) - Project overview and setup
- [Contributing](CONTRIBUTING.md) - Guidelines for contributors 