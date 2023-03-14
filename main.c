#include <math.h>
#include "gba.h"

#define SCR_W 160 // Width of gameboy screen in pixels
#define WIN_W 120 // Width of scaled down window
#define WIN_H 80 // Height of scaled down window
#define RGB(r, g, b) ((r) + ((g) << 5) + ((b) << 10)) // 15 bit, 0-31, 5bit=r, 5bit=g, 5bit=b

#define GRID_SIZE 10
#define WORLD_W WIN_W / GRID_SIZE
#define WORLD_H WIN_H / GRID_SIZE

int finalFrame = 0; // Final frame
int FPS = 0; // Current frame
int worldGrid[WORLD_H][WORLD_W] = 
{
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

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

            int i;
            int j;

            for (j = 0; j < WORLD_H; j++) {
                for (i = 0; i < WORLD_W; i++) {
                    if (worldGrid[j][i] == 1) {
                        drawCube(SCR_W * j * GRID_SIZE + i * GRID_SIZE, GRID_SIZE, 31, 31, 31);
                    } else {
                        drawCube(SCR_W * j * GRID_SIZE + i * GRID_SIZE, GRID_SIZE, 5, 5, 5);
                    }
                }
            }   
                
            // frames per second visual counter
            VRAM[(int)(((float) FPS / 15) * SCR_W)] = RGB(31, 0, 0);
            VRAM[(int)(((float) FPS / 15) * SCR_W) + 1] = RGB(31, 0, 0); 
            VRAM[(int) (SCR_W + (((float) FPS / 15) * SCR_W))] = RGB(31, 0, 0); 
            VRAM[(int) (SCR_W + (((float) FPS / 15) * SCR_W)) + 1] = RGB(31, 0, 0); 

            // draw player dot
            VRAM[P.y * SCR_W + P.x] = RGB(0, 20, 0); 
            VRAM[(P.y + 1) * SCR_W + P.x] = RGB(0, 20, 0); 
            VRAM[P.y * SCR_W + P.x + 1] = RGB(0, 20, 0); 
            VRAM[(P.y + 1) * SCR_W + P.x + 1] = RGB(0, 20, 0); 

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
