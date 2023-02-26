#pragma once
#include <nds.h>

enum Icons {
    ICON_NULL=0,
    ICON_PRINTER
};

extern volatile int dsFrameCounter;

extern int interruptWaitMode;
extern int scaleMode;
extern int scaleFilter;
extern u8 gfxMask;
extern volatile int loadedBorderType;
extern bool customBorderExists;
extern bool sgbBorderLoaded;

extern u8 bgPaletteData[0x40];
extern u8 sprPaletteData[0x40];

void doAtVBlank(void (*func)(void));

void drawScanline(int scanline);
void drawScanline_P2(int scanline);
void drawScreen();

void initGFX();
void initGFXPalette();
void refreshGFX();
void clearGFX();
void refreshSgbPalette();

void displayIcon(int iconid);

void selectBorder(); // Starts the file chooser for a border
int loadBorder(const char* filename); // Loads the border to vram
void checkBorder(); // Decides what kind of border to use, invokes loadBorder if necessary

void refreshScaleMode();
void setGFXMask(int mask);
void setSgbTiles(u8* src, u8 flags);
void setSgbMap(u8* src);

void writeVram(u16 addr, u8 val);
void writeVram16(u16 addr, u16 src);
void writeHram(u16 addr, u8 val);
void handleVideoRegister(u8 ioReg, u8 val);

enum {
    BORDER_NONE=0,
    BORDER_SGB,
    BORDER_CUSTOM
};  // Use with loadedBorderType

/* constants from the CGB ROM - https://github.com/ISSOtm/gb-bootroms/blob/45a21c573d7aec7efd15f8745479e09c30463bc7/src/cgb.asm#L1260-L1412 */
const u8 CGB_SELECT_IDX[] = {
    0x1C, 0x12, 0x10, 0x19, 0x18, 0x0D, 0x16, 0x17, 0x07, 0x1A, 0x05, 0x1C, 0x13
};

const u8 CGB_SELECT_SHUF[] = {
    0x03, 0x00, 0x05, 0x03, 0x05, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x03, 0x00
};

#define CGB_SELECT_NAMES {\
    "Default",\
    "Up",\
    "A + Up",\
    "B + Up",\
    "Left",\
    "A + Left",\
    "B + Left",\
    "Down",\
    "A + Down",\
    "B + Down",\
    "Right",\
    "A + Right",\
    "B + Right"\
}

const u8 CGB_PAL_OFFSETS[] = {
    16*4,   22*4,    8*4,
    17*4,    4*4,   13*4,
    27*4+3,  0*4,   14*4,
    27*4+3,  4*4,   15*4,
     4*4,    4*4,    7*4,

     4*4,   22*4,   18*4,
     4*4,   22*4,   20*4,
    28*4,   22*4,   24*4,
    19*4,   22*4+3,  9*4,
    16*4,   28*4,   10*4,

     3*4+3,  3*4+3, 11*4,
     4*4,   23*4,   28*4,
    17*4,   22*4,    2*4,
     4*4,    0*4,    2*4,
     4*4,   28*4,    3*4,

    28*4,    3*4,    0*4,
     3*4,   28*4,    4*4,
    21*4,   28*4,    4*4,
     3*4,   28*4,    0*4,
     4*4,    3*4,   27*4,

    25*4,    3*4,   28*4,
     0*4,   28*4,    8*4,
     5*4,    5*4,    5*4,
     3*4,   28*4,   12*4,
     4*4,    3*4,   28*4,

     0*4,    0*4,    1*4,
    28*4,    3*4,    6*4,
    26*4,   26*4,   26*4,
     4*4,   28*4,   29*4
};

const u16 CGB_PALETTES[] = {
    RGB15(0x1F,0x1F,0x1F), RGB15(0x1F,0x15,0x0C), RGB15(0x10,0x06,0x00), RGB15(0x00,0x00,0x00),
    RGB15(0x1F,0x1C,0x18), RGB15(0x19,0x13,0x10), RGB15(0x10,0x0D,0x05), RGB15(0x0B,0x06,0x01),
    RGB15(0x1F,0x1F,0x1F), RGB15(0x11,0x11,0x1B), RGB15(0x0A,0x0A,0x11), RGB15(0x00,0x00,0x00),
    RGB15(0x1F,0x1F,0x1F), RGB15(0x0F,0x1F,0x06), RGB15(0x00,0x10,0x00), RGB15(0x00,0x00,0x00), // "3 plus 3" begins at this black
    RGB15(0x1F,0x1F,0x1F), RGB15(0x1F,0x10,0x10), RGB15(0x12,0x07,0x07), RGB15(0x00,0x00,0x00),
    
    RGB15(0x1F,0x1F,0x1F), RGB15(0x14,0x14,0x14), RGB15(0x0A,0x0A,0x0A), RGB15(0x00,0x00,0x00),
    RGB15(0x1F,0x1F,0x1F), RGB15(0x1F,0x1F,0x00), RGB15(0x0F,0x09,0x00), RGB15(0x00,0x00,0x00),
    RGB15(0x1F,0x1F,0x1F), RGB15(0x0F,0x1F,0x00), RGB15(0x16,0x0E,0x00), RGB15(0x00,0x00,0x00),
    RGB15(0x1F,0x1F,0x1F), RGB15(0x15,0x15,0x10), RGB15(0x08,0x0E,0x0F), RGB15(0x00,0x00,0x00),
    RGB15(0x14,0x13,0x1F), RGB15(0x1F,0x1F,0x00), RGB15(0x00,0x0C,0x00), RGB15(0x00,0x00,0x00),

    RGB15(0x1F,0x1F,0x19), RGB15(0x0C,0x1D,0x1D), RGB15(0x13,0x10,0x06), RGB15(0x0B,0x0B,0x0B),
    RGB15(0x16,0x16,0x1F), RGB15(0x1F,0x1F,0x12), RGB15(0x15,0x0B,0x08), RGB15(0x00,0x00,0x00),
    RGB15(0x1F,0x1F,0x14), RGB15(0x1F,0x12,0x12), RGB15(0x12,0x12,0x1F), RGB15(0x00,0x00,0x00),
    RGB15(0x1F,0x1F,0x13), RGB15(0x12,0x16,0x1F), RGB15(0x0C,0x12,0x0E), RGB15(0x00,0x07,0x07),
    RGB15(0x0D,0x1F,0x00), RGB15(0x1F,0x1F,0x1F), RGB15(0x1F,0x0A,0x09), RGB15(0x00,0x00,0x00),

    RGB15(0x0A,0x1B,0x00), RGB15(0x1F,0x10,0x00), RGB15(0x1F,0x1F,0x00), RGB15(0x1F,0x1F,0x1F),
    RGB15(0x1F,0x1F,0x1F), RGB15(0x1F,0x0E,0x00), RGB15(0x12,0x08,0x00), RGB15(0x00,0x00,0x00),
    RGB15(0x1F,0x18,0x08), RGB15(0x1F,0x1A,0x00), RGB15(0x12,0x07,0x00), RGB15(0x09,0x00,0x00),
    RGB15(0x1F,0x1F,0x1F), RGB15(0x0A,0x1F,0x00), RGB15(0x1F,0x08,0x00), RGB15(0x00,0x00,0x00),
    RGB15(0x1F,0x0C,0x0A), RGB15(0x1A,0x00,0x00), RGB15(0x0C,0x00,0x00), RGB15(0x00,0x00,0x00),

    RGB15(0x1F,0x1F,0x1F), RGB15(0x1F,0x13,0x00), RGB15(0x1F,0x00,0x00), RGB15(0x00,0x00,0x00),
    RGB15(0x1F,0x1F,0x1F), RGB15(0x00,0x1F,0x00), RGB15(0x06,0x10,0x00), RGB15(0x00,0x09,0x00),
    RGB15(0x1F,0x1F,0x1F), RGB15(0x0B,0x17,0x1F), RGB15(0x1F,0x00,0x00), RGB15(0x00,0x00,0x1F), // "22 plus 3" begins at this blue
    RGB15(0x1F,0x1F,0x1F), RGB15(0x1F,0x1F,0x0F), RGB15(0x00,0x10,0x1F), RGB15(0x1F,0x00,0x00),
    RGB15(0x1F,0x1F,0x1F), RGB15(0x1F,0x1F,0x00), RGB15(0x1F,0x00,0x00), RGB15(0x00,0x00,0x00),

    RGB15(0x1F,0x1F,0x00), RGB15(0x1F,0x00,0x00), RGB15(0x0C,0x00,0x00), RGB15(0x00,0x00,0x00),
    RGB15(0x1F,0x1F,0x1F), RGB15(0x1F,0x19,0x00), RGB15(0x13,0x0C,0x00), RGB15(0x00,0x00,0x00),
    RGB15(0x00,0x00,0x00), RGB15(0x00,0x10,0x10), RGB15(0x1F,0x1B,0x00), RGB15(0x1F,0x1F,0x1F), // "27 plus 6" begins at this white
    RGB15(0x1F,0x1F,0x1F), RGB15(0x0C,0x14,0x1F), RGB15(0x00,0x00,0x1F), RGB15(0x00,0x00,0x00),
    RGB15(0x1F,0x1F,0x1F), RGB15(0x0F,0x1F,0x06), RGB15(0x00,0x0C,0x18), RGB15(0x00,0x00,0x00)
};