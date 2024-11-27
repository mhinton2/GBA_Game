/*
 * game over screen program
 */

#include <stdio.h>

/* include the image we are using */
#include "map3.h"

/* the control registers for the four tile layers */
volatile unsigned short* bg0_control2 = (volatile unsigned short*) 0x4000008;
volatile unsigned short* bg1_control2 = (volatile unsigned short*) 0x400000a;
volatile unsigned short* bg2_control2 = (volatile unsigned short*) 0x400000c;
volatile unsigned short* bg3_control2 = (volatile unsigned short*) 0x400000e;

/* the three tile modes */
#define MODE0 0x00
#define MODE1 0x01
#define MODE2 0x02

/* enable bits for the four tile layers */
#define BG0_ENABLE 0x100
#define BG1_ENABLE 0x200
#define BG2_ENABLE 0x400
#define BG3_ENABLE 0x800

/* palette is always 256 colors */
#define PALETTE_SIZE 256

/* flag for turning on DMA */
#define DMA_ENABLE 0x80000000

/* flags for the sizes to transfer, 16 or 32 bits */
#define DMA_16 0x00000000
#define DMA_32 0x04000000

/* pointer to the DMA source location */
volatile unsigned int* dma_source2 = (volatile unsigned int*) 0x40000D4;

/* pointer to the DMA destination location */
volatile unsigned int* dma_destination2 = (volatile unsigned int*) 0x40000D8;

/* pointer to the DMA count/control */
volatile unsigned int* dma_count2 = (volatile unsigned int*) 0x40000DC;

/* copy data using DMA */
void memcpy16_dma2(unsigned short* dest, unsigned short* source, int amount) {
    *dma_source2 = (unsigned int) source;
    *dma_destination2 = (unsigned int) dest;
    *dma_count2 = amount | DMA_16 | DMA_ENABLE;
}

#define WIDTH 240
#define HEIGHT 160


/* the display control pointer points to the gba graphics register */
volatile unsigned long* display_control2 = (volatile unsigned long*) 0x4000000;

/* the address of the color palette */
volatile unsigned short* map3_palette2 = (volatile unsigned short*) 0x5000000;

/* return a pointer to one of the 4 character blocks (0-3) */
volatile unsigned short* char_block2(unsigned long block) {
    /* they are each 16K big */
    return (volatile unsigned short*) (0x6000000 + (block * 0x4000));
}

/* return a pointer to one of the 32 screen blocks (0-31) */
volatile unsigned short* screen_block2(unsigned long block) {
    /* they are each 2K big */
    return (volatile unsigned short*) (0x6000000 + (block * 0x800));
}


/* function to setup background 0 for this program */
void setup_background2() {
    /* load the palette from the image into palette memory*/
    memcpy16_dma2((unsigned short*) map3_palette2, (unsigned short*) map3_palette2, PALETTE_SIZE);

    /* load the image into char block 0 */
    memcpy16_dma2((unsigned short*) char_block2(0), (unsigned short*) map3_data,
            (map3_width * map3_height) / 2);

    /* bg0 is just all black so the pink does not show through! */
    *bg0_control2 = 3 |    /* priority, 0 is highest, 3 is lowest */
        (0 << 2)  |       /* the char block the image data is stored in */
        (0 << 6)  |       /* the mosaic flag */
        (1 << 7)  |       /* color mode, 0 is 16 colors, 1 is 256 colors */
        (16 << 8) |       /* the screen block the tile data is stored in */
        (1 << 13) |       /* wrapping flag */
        (0 << 14);        /* bg size, 0 is 256x256 */


    /* bg1 is our actual text background */
    *bg1_control2 = 0 |    /* priority, 0 is highest, 3 is lowest */
        (0 << 2)  |       /* the char block the image data is stored in */
        (0 << 6)  |       /* the mosaic flag */
        (1 << 7)  |       /* color mode, 0 is 16 colors, 1 is 256 colors */
        (24 << 8) |       /* the screen block the tile data is stored in */
        (1 << 13) |       /* wrapping flag */
        (0 << 14);        /* bg size, 0 is 256x256 */

    /* clear the tile map in screen block 16 to all black tile*/
    volatile unsigned short* ptr = screen_block2(16);
    for (int i = 0; i < 32 * 32; i++) {
        ptr[i] = 95;
    }

    /* clear the text map to be all blanks */
    ptr = screen_block2(24);
    for (int i = 0; i < 32 * 32; i++) {
        ptr[i] = 0;
    }
}


/* function to set text on the screen at a given location */
void set_text(char* str, int row, int col) {                    
    /* find the index in the texmap to draw to */
    int index = row * 32 + col;

    /* the first 32 characters are missing from the map (controls etc.) */
    int missing = 32; 

    /* pointer to text map */
    volatile unsigned short* ptr = screen_block2(24);

    /* for each character */
    while (*str) {
        /* place this character in the map */
        ptr[index] = *str - missing;

        /* move onto the next character */
        index++;
        str++;
    }   
}

/* declaration of assembly function to uppercase a string */
void uppercase(char* s);

/* the main function */
void gameoverscreen(int gamescore) {
    /* we set the mode to mode 0 with bg0 and bg1 on */
    *display_control2 = MODE0 | BG0_ENABLE | BG1_ENABLE;

    /* setup the background 0 */
    setup_background2();

    /* call assembly function */
	char msg[10];
	gamescore = gamescore / 1000;
    sprintf(msg, "Score = %d", gamescore);
    set_text(msg, 0, 0);

    /* loop forever */
    while (1) {
    }
}

