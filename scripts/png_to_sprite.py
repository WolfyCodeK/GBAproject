#!/usr/bin/env python3
"""
PNG to GBA Sprite Array Converter
Converts PNG images to C array format for Game Boy Advance development
"""

from PIL import Image
import sys
import os

# Project paths for organized structure
SPRITES_HEADER = "../src/sprites.h"
COLORS_HEADER = "../src/colors.h"
MAIN_C_FILE = "../src/main.c"

def rgb_to_gba(r, g, b):
    """Convert 0-255 RGB to 0-31 GBA RGB format"""
    return (r // 8, g // 8, b // 8)

def gba_to_rgb_macro(r, g, b):
    """Convert GBA RGB to RGB macro format"""
    return f"RGB({r}, {g}, {b})"

def find_closest_palette_color(pixel_rgb, palette):
    """Find the closest color in the palette for a given RGB value"""
    if len(pixel_rgb) == 4 and pixel_rgb[3] < 128:  # Transparent pixel
        return 0
    
    r, g, b = pixel_rgb[:3]
    gba_rgb = rgb_to_gba(r, g, b)
    
    min_distance = float('inf')
    closest_index = 0
    
    for i, (pr, pg, pb) in enumerate(palette):
        if i == 0:  # Skip transparent color
            continue
        distance = ((gba_rgb[0] - pr) ** 2 + 
                   (gba_rgb[1] - pg) ** 2 + 
                   (gba_rgb[2] - pb) ** 2) ** 0.5
        if distance < min_distance:
            min_distance = distance
            closest_index = i
    
    return closest_index

def extract_palette(image, max_colors=5):
    """Extract a color palette from the image"""
    # Convert to RGBA if not already
    if image.mode != 'RGBA':
        image = image.convert('RGBA')
    
    # Get unique colors
    colors = set()
    pixels = list(image.getdata())
    
    for pixel in pixels:
        if len(pixel) == 4 and pixel[3] < 128:  # Transparent
            continue
        colors.add(rgb_to_gba(pixel[0], pixel[1], pixel[2]))
    
    # Start with transparent
    palette = [(0, 0, 0)]  # Index 0 = transparent
    
    # Add the most common colors (simplified - just takes first ones found)
    for color in list(colors)[:max_colors-1]:
        palette.append(color)
    
    # Fill remaining slots with black if needed
    while len(palette) < max_colors:
        palette.append((0, 0, 0))
    
    return palette

def convert_png_to_sprite(png_path, sprite_name=None, max_colors=5):
    """Convert PNG to sprite array"""
    try:
        # Load image
        image = Image.open(png_path)
        
        # Get sprite name from filename if not provided
        if sprite_name is None:
            sprite_name = os.path.splitext(os.path.basename(png_path))[0]
        
        # Convert to RGBA
        if image.mode != 'RGBA':
            image = image.convert('RGBA')
        
        width, height = image.size
        
        # Extract palette
        palette = extract_palette(image, max_colors)
        
        # Convert pixels to palette indices
        pixels = list(image.getdata())
        sprite_data = []
        
        for y in range(height):
            row = []
            for x in range(width):
                pixel = pixels[y * width + x]
                palette_index = find_closest_palette_color(pixel, palette)
                row.append(palette_index)
            sprite_data.append(row)
        
        return sprite_data, palette, width, height, sprite_name
        
    except Exception as e:
        print(f"Error converting {png_path}: {e}")
        return None

def generate_sprite_code(sprite_data, width, height, sprite_name):
    """Generate C code for just the sprite array"""
    c_code = []
    
    # Generate sprite array
    c_code.append(f"// {sprite_name} sprite ({width}x{height})")
    c_code.append(f"const int {sprite_name}[{height}][{width}] = {{")
    
    for y, row in enumerate(sprite_data):
        row_str = "    {" + ", ".join(map(str, row)) + "}"
        if y < len(sprite_data) - 1:
            row_str += ","
        c_code.append(row_str)
    
    c_code.append("};")
    c_code.append("")
    
    return "\n".join(c_code)

def generate_color_code(palette, sprite_name):
    """Generate C code for just the color palette"""
    c_code = []
    
    # Generate color palette
    c_code.append(f"// {sprite_name} colors")
    c_code.append(f"const int {sprite_name}Colors[{len(palette)}] = {{")
    
    for i, (r, g, b) in enumerate(palette):
        color_comment = ""
        if i == 0:
            color_comment = " // transparent"
        else:
            color_comment = f" // color {i}"
        
        color_str = f"    {gba_to_rgb_macro(r, g, b)}"
        if i < len(palette) - 1:
            color_str += ","
        color_str += color_comment
        c_code.append(color_str)
    
    c_code.append("};")
    c_code.append("")
    
    return "\n".join(c_code)

def add_to_main_c(sprite_code, color_code, sprite_name):
    """Add sprite and color definitions to main.c"""
    try:
        # Read main.c
        with open(MAIN_C_FILE, 'r') as f:
            content = f.read()
        
        # Find insertion points
        sprite_marker = "// ===== SPRITE DEFINITIONS ====="
        color_marker = "// ===== COLOR DEFINITIONS ====="
        
        if sprite_marker in content and color_marker in content:
            # Insert sprite after sprite marker
            sprite_pos = content.find(sprite_marker) + len(sprite_marker)
            content = content[:sprite_pos] + "\n\n" + sprite_code + content[sprite_pos:]
            
            # Insert color after color marker (adjust position due to sprite insertion)
            color_pos = content.find(color_marker) + len(color_marker)
            content = content[:color_pos] + "\n\n" + color_code + content[color_pos:]
            
            # Write back
            with open(MAIN_C_FILE, 'w') as f:
                f.write(content)
            
            return True
        else:
            print("Warning: Could not find insertion markers in main.c")
            return False
    except Exception as e:
        print(f"Error updating main.c: {e}")
        return False

def add_extern_declarations(sprite_name, width, height):
    """Add extern declarations to header files"""
    try:
        # Add to sprites.h
        with open(SPRITES_HEADER, 'r') as f:
            sprites_content = f.read()
        
        extern_decl = f"extern const int {sprite_name}[{height}][{width}];\n"
        insertion_point = sprites_content.rfind("#endif")
        if insertion_point != -1:
            sprites_content = sprites_content[:insertion_point] + extern_decl + "\n" + sprites_content[insertion_point:]
            with open(SPRITES_HEADER, 'w') as f:
                f.write(sprites_content)
        
        # Add to colors.h
        with open(COLORS_HEADER, 'r') as f:
            colors_content = f.read()
        
        extern_decl = f"extern const int {sprite_name}Colors[5];\n"
        insertion_point = colors_content.rfind("#endif")
        if insertion_point != -1:
            colors_content = colors_content[:insertion_point] + extern_decl + "\n" + colors_content[insertion_point:]
            with open(COLORS_HEADER, 'w') as f:
                f.write(colors_content)
        
        return True
    except Exception as e:
        print(f"Error updating headers: {e}")
        return False

def main():
    if len(sys.argv) < 2:
        print("Usage: python png_to_sprite.py <image.png> [sprite_name] [max_colors]")
        print("Example: python png_to_sprite.py ../res/player.png playerSprite 5")
        print("Run from scripts/ directory")
        sys.exit(1)
    
    png_path = sys.argv[1]
    sprite_name = sys.argv[2] if len(sys.argv) > 2 else None
    max_colors = int(sys.argv[3]) if len(sys.argv) > 3 else 5
    
    if not os.path.exists(png_path):
        print(f"Error: File {png_path} not found")
        sys.exit(1)
    
    print(f"Converting {png_path}...")
    
    result = convert_png_to_sprite(png_path, sprite_name, max_colors)
    if result is None:
        print("Conversion failed!")
        sys.exit(1)
    
    sprite_data, palette, width, height, sprite_name = result
    
    print(f"Successfully converted {width}x{height} sprite with {len(palette)} colors")
    
    # Generate code
    sprite_code = generate_sprite_code(sprite_data, width, height, sprite_name)
    color_code = generate_color_code(palette, sprite_name)
    
    # Update files
    main_success = add_to_main_c(sprite_code, color_code, sprite_name)
    header_success = add_extern_declarations(sprite_name, width, height)
    
    if main_success:
        print("✓ Added definitions to src/main.c")
    else:
        print("✗ Failed to update src/main.c")
    
    if header_success:
        print("✓ Added declarations to headers")
    else:
        print("✗ Failed to update headers")
    
    # Determine sprite size for function recommendation
    if width == 8 and height == 8:
        func_name = "drawSprite8x8"
    elif width == 16 and height == 16:
        func_name = "drawSprite16x16"
    else:
        func_name = f"drawSprite{width}x{height} // You'll need to create this function"
    
    print(f"\n📝 Usage in main.c:")
    print(f"{func_name}(x, y, {sprite_name}, {sprite_name}Colors);")
    
    print("\n" + "="*50)
    print("Generated sprite code:")
    print("="*50)
    print(sprite_code)
    print("Generated color code:")
    print("="*50)
    print(color_code)

if __name__ == "__main__":
    main() 