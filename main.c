#include <math.h>
#include "gba.h"

#define SCR_W 240
#define SCR_H 160
#define WIN_W 120
#define WIN_H 80
#define RGB(r, g, b) ((r) + ((g) << 5) + ((b) << 10))
#define GRID_SIZE 10
#define WORLD_W (WIN_W / GRID_SIZE)
#define WORLD_H (WIN_H / GRID_SIZE)

#define PLAYER_MOVE 2

int finalFrame = 0;
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

void clearBackground()
{
    u32* vram = (u32*) VRAM;
    u32 color = RGB(0, 0, 0);

    for (int i = 0; i < (SCR_W * SCR_H) / 2; i++) {
        vram[i] = color;
    }
}

void buttons() 
{
    if (KEY_R) P.x = MIN(P.x + PLAYER_MOVE, WIN_W - 1);
    if (KEY_L) P.x = MAX(P.x - PLAYER_MOVE, 0);
    if (KEY_U) P.y = MAX(P.y - PLAYER_MOVE, 0);
    if (KEY_D) P.y = MIN(P.y + PLAYER_MOVE, WIN_H - 1);
}

void initGameVars()
{
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
    // Init mode 5
    *(volatile u16 *)0x4000000 = 0x405; // mode 5 background 2
    *(volatile u16 *)0x400010A = 0x82;  // enable timer for fps
    *(volatile u16 *)0x400010E = 0x84;  // cnt timer overflow
    // scale small mode 5 screen to full screen
    REG_BG2PA = 128; // 128=scale
    REG_BG2PD = 128; // 128=scale

    initGameVars();

    while (1)
    {
        // draw 15 frames a second
        if (REG_TM2D >> 12 != finalFrame) 
        {
            clearBackground();                        
            buttons();
            int i, j;
            for (j = 0; j < WORLD_H; j++) {
                for (i = 0; i < WORLD_W; i++) {
                    drawCube(SCR_W * j * GRID_SIZE + i * GRID_SIZE, GRID_SIZE, 
                        (worldGrid[j][i] == 1) ? 31 : 5, 
                        (worldGrid[j][i] == 1) ? 31 : 5, 
                        (worldGrid[j][i] == 1) ? 31 : 5);
                }
            }   
                
            // frames per second visual counter
            int fps_pos = (int)(((float) FPS / 15) * SCR_W);
            VRAM[fps_pos] = VRAM[fps_pos + 1] = VRAM[SCR_W + fps_pos] = VRAM[SCR_W + fps_pos + 1] = RGB(31, 0, 0);

            // draw player dot
            int p_pos = P.y * SCR_W + P.x;
            VRAM[p_pos] = VRAM[p_pos + 1] = VRAM[SCR_W + p_pos] = VRAM[SCR_W + p_pos + 1] = RGB(0, 20, 0); 

            // increase frame
            FPS++;

            // reset counter
            if (finalFrame > REG_TM2D >> 12)
            {
                FPS = 0;
            }                        
            finalFrame = REG_TM2D >> 12; 

            // swap front and back buffers
            while (*Scanline < 160) {} // wait all scanlines
            if (DISPCNT & BACKB)
            {
                DISPCNT &= ~BACKB;
                VRAM = (volatile u16 *)VRAM_B;
            } 
            else
            {
                DISPCNT |= BACKB;
                VRAM = (volatile u16 *)VRAM_F;
            } 
        }
    }
}

