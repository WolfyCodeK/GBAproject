#include <math.h>
#include "gba.h"
#include "sprites.h"
#include "colors.h"

#define SCR_W 160 // Width of gameboy screen in pixels
#define WIN_W 120 // Width of scaled down window
#define WIN_H 80 // Height of scaled down window
#define RGB(r, g, b) ((r) + ((g) << 5) + ((b) << 10)) // 15 bit, 0-31, 5bit=r, 5bit=g, 5bit=b

int finalFrame = 0; // Final frame
int FPS = 0; // Current frame
int fpsCounter = 0; // Frame counter for FPS calculation
int fpsDisplayValue = 15; // Current FPS display value

typedef struct // player
{
    int x, y; // position
} Player;

Player P;

// clears all 120x80 pixels
void clearBackground()
{
    int x, y;

    for (x = 0; x < WIN_W; x++)
    {
        for (y = 0; y < WIN_H; y++)
        {
            VRAM[y * SCR_W + x] = RGB(0, 0, 0);
        } 
    }
}

// buttons to press
void buttons() 
{
    // move right
    if (KEY_R)
    {
        P.x += 2;
        if (P.x > WIN_W - 1)
        {
            P.x = WIN_W - 1;
        }
    } 

    // move left
    if (KEY_L)
    {
        P.x -= 2;
        if (P.x < 0)
        {
            P.x = 0;
        }
    } 

    // move up
    if (KEY_U)
    {
        P.y -= 2;
        if (P.y < 0)
        {
            P.y = 0;
        }
    } 

    // move down
    if (KEY_D)
    {
        P.y += 2;
        if (P.y > WIN_H - 1)
        {
            P.y = WIN_H - 1;
        }
    } 

    if (KEY_A)
    {
    }
    
    if (KEY_B)
    {
    }

    if (KEY_LS)
    {
    }

    if (KEY_RS)
    {
    }

    if (KEY_ST)
    {
    }

    if (KEY_SL)
    {
    }
}

// Initialize variables
void initGameVars()
{
    // init player
    P.x = 70;
    P.y = 35; 
}

void drawCube(int topLeftPos, int size, int r, int g, int b) {
    int i;
    int j;

    for (j = 0; j < size - 1; j++) {
        for(i = 0; i < size - 1; i++) {
            VRAM[j * SCR_W + (topLeftPos + i)] = RGB(r, g, b);
        }
    }
}

// Font and sprite definitions moved to sprites.h

// Draw a single digit at position (x, y)
void drawDigit(int x, int y, int digit, int r, int g, int b) {
    if (digit < 0 || digit > 9) return;
    
    int row, col;
    for (row = 0; row < 5; row++) {
        for (col = 0; col < 3; col++) {
            if (digitFont[digit][row] & (1 << (2 - col))) {
                int pixelX = x + col;
                int pixelY = y + row;
                if (pixelX < WIN_W && pixelY < WIN_H) {
                    VRAM[pixelY * SCR_W + pixelX] = RGB(r, g, b);
                }
            }
        }
    }
}

// Draw a 2-digit number at position (x, y)
void drawFPS(int x, int y, int fps, int r, int g, int b) {
    int tens = fps / 10;
    int ones = fps % 10;
    
    // Draw tens digit
    if (tens > 0) {
        drawDigit(x, y, tens, r, g, b);
    }
    
    // Draw ones digit
    drawDigit(x + 4, y, ones, r, g, b);
}

// ===== SPRITE DEFINITIONS =====

// Simple player character sprite (8x8)
const int playerSprite[8][8] = {
    {0, 0, 1, 1, 1, 1, 0, 0},  // Head outline
    {0, 1, 2, 2, 2, 2, 1, 0},  // Head with skin
    {1, 2, 3, 2, 2, 3, 2, 1},  // Eyes
    {1, 2, 2, 2, 2, 2, 2, 1},  // Face
    {1, 2, 4, 2, 2, 4, 2, 1},  // Mouth/cheeks
    {0, 1, 1, 1, 1, 1, 1, 0},  // Body outline
    {0, 0, 1, 2, 2, 1, 0, 0},  // Body
    {0, 0, 1, 0, 0, 1, 0, 0}   // Legs
};

// Simple 3x5 bitmap font for digits 0-9
const int digitFont[10][5] = {
    {0x7, 0x5, 0x5, 0x5, 0x7}, // 0
    {0x2, 0x6, 0x2, 0x2, 0x7}, // 1
    {0x7, 0x1, 0x7, 0x4, 0x7}, // 2
    {0x7, 0x1, 0x7, 0x1, 0x7}, // 3
    {0x5, 0x5, 0x7, 0x1, 0x1}, // 4
    {0x7, 0x4, 0x7, 0x1, 0x7}, // 5
    {0x7, 0x4, 0x7, 0x5, 0x7}, // 6
    {0x7, 0x1, 0x1, 0x1, 0x1}, // 7
    {0x7, 0x5, 0x7, 0x5, 0x7}, // 8
    {0x7, 0x5, 0x7, 0x1, 0x7}  // 9
};

// dirt sprite (16x16)
const int dirt[16][16] = {
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 3, 3, 3, 3, 3},
    {3, 2, 2, 3, 1, 1, 4, 4, 2, 2, 2, 2, 4, 4, 3, 3},
    {3, 2, 2, 2, 1, 1, 4, 4, 3, 1, 1, 1, 4, 4, 2, 3},
    {4, 4, 1, 3, 3, 2, 2, 3, 3, 1, 1, 1, 2, 2, 2, 3},
    {4, 4, 3, 1, 2, 2, 2, 3, 4, 4, 3, 2, 2, 2, 3, 3},
    {3, 3, 3, 2, 2, 1, 1, 3, 4, 4, 2, 2, 2, 3, 3, 3},
    {3, 4, 4, 3, 4, 1, 1, 3, 3, 2, 2, 2, 3, 4, 4, 3},
    {2, 4, 4, 2, 4, 4, 3, 1, 1, 3, 3, 3, 3, 4, 4, 3},
    {2, 2, 3, 2, 2, 3, 2, 4, 1, 3, 1, 1, 3, 3, 3, 3},
    {1, 1, 3, 3, 3, 2, 2, 2, 3, 3, 1, 1, 3, 1, 1, 3},
    {1, 1, 3, 1, 1, 3, 3, 3, 3, 3, 3, 3, 2, 1, 1, 3},
    {3, 4, 4, 1, 1, 3, 3, 3, 4, 4, 3, 2, 2, 2, 2, 2},
    {3, 4, 4, 3, 3, 2, 1, 1, 4, 4, 2, 2, 4, 4, 2, 2},
    {3, 3, 1, 1, 3, 2, 1, 1, 3, 2, 2, 2, 4, 4, 3, 3},
    {3, 3, 1, 1, 2, 2, 2, 2, 2, 4, 4, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 2, 2, 2, 2, 2, 4, 4, 3, 3, 3, 3, 3}
};

// ===== COLOR DEFINITIONS =====

// Player colors
const int playerColors[5] = {
    RGB(0, 0, 0),      // 0 = transparent
    RGB(8, 4, 2),      // 1 = dark brown (outline)
    RGB(25, 20, 15),   // 2 = skin tone
    RGB(31, 31, 31),   // 3 = white (eyes)
    RGB(31, 8, 8)      // 4 = red (details)
};

// Common game colors
const int gameColors[8] = {
    RGB(0, 0, 0),      // Black
    RGB(31, 31, 31),   // White
    RGB(31, 0, 0),     // Red
    RGB(0, 31, 0),     // Green
    RGB(0, 0, 31),     // Blue
    RGB(31, 31, 0),    // Yellow
    RGB(31, 0, 31),    // Magenta
    RGB(0, 31, 31)     // Cyan
};

// Wall and floor colors for world rendering
const int worldColors[3] = {
    RGB(5, 5, 5),      // Dark floor
    RGB(31, 31, 31),   // White walls
    RGB(15, 10, 5)     // Medium brown
};

// dirt colors
const int dirtColors[5] = {
    RGB(0, 0, 0),      // transparent
    RGB(22, 28, 3),    // color 1
    RGB(17, 0, 2),     // color 2
    RGB(23, 15, 10),   // color 3
    RGB(4, 22, 9)      // color 4
};

// Draw sprite with custom colors - for 8x8 sprites
void drawSprite8x8(int x, int y, const int sprite[8][8], const int colors[]) {
    int i, j;
    
    for (j = 0; j < 8; j++) {
        for (i = 0; i < 8; i++) {
            int spriteX = x + i;
            int spriteY = y + j;
            
            // Check bounds
            if (spriteX >= 0 && spriteX < WIN_W && spriteY >= 0 && spriteY < WIN_H) {
                int colorIndex = sprite[j][i];
                
                // Don't draw transparent pixels (color index 0)
                if (colorIndex != 0) {
                    VRAM[spriteY * SCR_W + spriteX] = colors[colorIndex];
                }
            }
        }
    }
}

// Draw sprite with custom colors - for 16x16 sprites
void drawSprite16x16(int x, int y, const int sprite[16][16], const int colors[]) {
    int i, j;
    
    for (j = 0; j < 16; j++) {
        for (i = 0; i < 16; i++) {
            int spriteX = x + i;
            int spriteY = y + j;
            
            // Check bounds
            if (spriteX >= 0 && spriteX < WIN_W && spriteY >= 0 && spriteY < WIN_H) {
                int colorIndex = sprite[j][i];
                
                // Don't draw transparent pixels (color index 0)
                if (colorIndex != 0) {
                    VRAM[spriteY * SCR_W + spriteX] = colors[colorIndex];
                }
            }
        }
    }
}

int main()
{
    int x, y;

    // Init mode 5
    *(u16 *)0x4000000 = 0x405; // mode 5 background 2
    *(u16 *)0x400010A = 0x82;  // enable timer for fps
    *(u16 *)0x400010E = 0x84;  // cnt timer overflow

    // scale small mode 5 screen to full screen
    REG_BG2PA = 256 / 2; // 256=normal 128=scale
    REG_BG2PD = 256 / 2; // 256=normal 128=scale

    initGameVars();

    while (1)
    {
        // draw 15 frames a second
        if (REG_TM2D >> 12 != finalFrame) 
        {
            clearBackground();                        
            buttons();
                
            // Calculate actual FPS based on frame timing
            fpsCounter++;
            
            // Every 8 frames, calculate FPS based on timer difference
            if (fpsCounter >= 8) {
                int currentTime = REG_TM2D >> 12;
                static int lastTime = 0;
                int timeDiff = currentTime - lastTime;
                
                if (timeDiff > 0) {
                    fpsDisplayValue = (8 * 60) / timeDiff; // Rough FPS calculation
                    if (fpsDisplayValue > 99) fpsDisplayValue = 99; // Cap at 99
                    if (fpsDisplayValue < 1) fpsDisplayValue = 1;   // Minimum 1
                }
                
                lastTime = currentTime;
                fpsCounter = 0;
            }
            
            // Draw FPS in top right corner (position: 108, 2) - dark blue color for contrast
            drawFPS(108, 2, fpsDisplayValue, 0, 0, 20); // Dark blue color
            
            // frames per second visual counter (keep the original red pixel)
            VRAM[(int)(((float) FPS / 15) * SCR_W)] = RGB(31, 0, 0);
            VRAM[(int)(((float) FPS / 15) * SCR_W) + 1] = RGB(31, 0, 0); 
            VRAM[(int) (SCR_W + (((float) FPS / 15) * SCR_W))] = RGB(31, 0, 0); 
            VRAM[(int) (SCR_W + (((float) FPS / 15) * SCR_W)) + 1] = RGB(31, 0, 0); 

            // draw dirt sprite
            drawSprite16x16(20, 20, dirt, dirtColors);

            // Draw player character sprite at player position
            drawSprite8x8(P.x - 4, P.y - 4, playerSprite, playerColors);
            

            // increase frame
            FPS += 1;

            // reset counter
            if (finalFrame > REG_TM2D >> 12)
            {
                FPS = 0;
            }                        

            finalFrame = REG_TM2D >> 12; 

            // swap front and back buffers
            while (*Scanline < 160) // wait all scanlines
            {
            } 

            // back  buffer 
            if (DISPCNT & BACKB)
            {
                DISPCNT &= ~BACKB;
                VRAM = (u16 *)VRAM_B;
            } 
            else // front buffer
            {
                DISPCNT |= BACKB;
                VRAM = (u16 *)VRAM_F;
            } 
        }
    }
}
