#include <nds.h>
#include <fatfs.h>

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

#include "inputhelper.h"
#include "mmu.h"
#include "gameboy.h"
#include "main.h"
#include "console.h"
#include "gbcpu.h"
#include "nifi.h"
#include "gbgfx.h"
#include "gbsnd.h"
#include "cheats.h"
#include "sgb.h"
#include "gbs.h"
#include "common.h"
#include "filechooser.h"

#define FAT_CACHE_SIZE 16

FILE* romFile=NULL;
FILE* saveFile=NULL;
char filename[100];
char savename[100];
char basename[100];
char romTitle[20];

char* romPath = NULL;
char* biosPath = NULL;
char* borderPath = NULL;

std::vector<PaletteEntry> customPalettes;

const std::vector<PaletteEntry> defaultPalettes = {
    {"Grayscale",          {RGB15(31, 31, 31), RGB15(20, 20, 20), RGB15(13, 13, 13), RGB15( 5,  5,  5)}},
    {"Reversed",           {RGB15( 5,  5,  5), RGB15(13, 13, 13), RGB15(20, 20, 20), RGB15(31, 31, 31)}},
    {"AYY4",               {RGB15(29, 29, 27), RGB15(31, 25, 18), RGB15(31, 14, 14), RGB15( 0,  6,  7)}},
    {"SpaceHaze",          {RGB15(30, 28, 24), RGB15(25,  6, 18), RGB15(13,  4, 22), RGB15( 1,  1,  6)}},
    {"CRTGB",              {RGB15(27, 29,  4), RGB15( 9, 19,  2), RGB15( 1,  8,  1), RGB15( 1,  1,  0)}},
    {"Amber CRTGB",        {RGB15(31, 25,  3), RGB15(26, 10,  0), RGB15(11,  2,  2), RGB15( 2,  0,  1)}},
    {"Kirby SGB",          {RGB15(30, 23, 30), RGB15(28, 16, 16), RGB15(14,  6, 28), RGB15( 5,  5, 18)}},
    {"CherryMelon",        {RGB15(31, 27, 28), RGB15(31,  9, 13), RGB15( 5, 11,  6), RGB15( 0,  5,  4)}},
    {"Pumpkin GB",         {RGB15(30, 27, 15), RGB15(30, 13,  3), RGB15( 3, 13,  5), RGB15( 2,  5,  4)}},
    {"Purpledawn",         {RGB15(29, 31, 29), RGB15(19, 15, 23), RGB15( 5, 14, 15), RGB15( 0,  3,  6)}},
    {"Royal4",             {RGB15(29, 27, 11), RGB15(26, 16,  9), RGB15(17,  4, 21), RGB15(10,  2, 18)}},
    {"Grand Dad 4",        {RGB15(28, 30, 30), RGB15(12, 22, 30), RGB15(26,  7,  9), RGB15( 9,  3,  5)}},
    {"Mural GB",           {RGB15(30, 31, 31), RGB15(25, 21, 13), RGB15(20, 10,  6), RGB15( 1,  3,  9)}},
    {"Ocean GB",           {RGB15(17, 27, 30), RGB15( 7, 15, 27), RGB15( 5,  6, 17), RGB15( 3,  3,  6)}},
    {"Alleyway",           {RGB15(31, 26,  0), RGB15(31, 13, 31), RGB15(15, 15, 25), RGB15( 8,  8,  8)}},
    {"Pocket",             {RGB15(28, 28, 24), RGB15(24, 24, 20), RGB15(17, 17, 12), RGB15(13, 13,  9)}},
    {"Kadabura4",          {RGB15(31, 31, 31), RGB15(27,  0,  1), RGB15(11, 11, 11), RGB15( 0,  0,  0)}},
    {"Virtual",            {RGB15(31,  0,  0), RGB15(15,  0,  0), RGB15( 8,  0,  0), RGB15( 0,  0,  0)}},
    {"Love! Love!",        {RGB15(31, 31, 31), RGB15(31, 22, 28), RGB15(31, 12, 21), RGB15(21,  2,  6)}},
    {"Metroid II SGB",     {RGB15(21, 27,  4), RGB15(22,  4, 11), RGB15( 0, 15, 12), RGB15( 5,  3,  0)}},
    {"Micro 86",           {RGB15(31, 26, 22), RGB15(31, 15,  6), RGB15(31,  0,  0), RGB15( 5,  0,  2)}},
    {"Vivid 2Bit Scream",  {RGB15(25, 30,  6), RGB15(14, 21,  6), RGB15(11, 10, 20), RGB15(10,  4,  3)}},
    {"Pastel GBC/SGB",     {RGB15(31, 30, 21), RGB15(29, 17, 17), RGB15(19, 18, 30), RGB15( 0,  0,  0)}},
    {"Scold 2 bit",        {RGB15(26,  1,  1), RGB15( 2, 22,  0), RGB15(25,  0, 18), RGB15( 2,  3, 10)}},
    {"trans flag",         {RGB15(31, 31, 31), RGB15(30, 21, 22), RGB15(11, 25, 30), RGB15( 4,  4,  4)}},
    {"strawberry parfait", {RGB15(31, 28, 25), RGB15(30, 10, 26), RGB15(26,  4,  6), RGB15( 4,  2,  0)}},
    {"bric-a-brac",        {RGB15(22, 26, 27), RGB15(30, 18,  3), RGB15(29, 10,  7), RGB15( 1,  5,  7)}},
    {"poolors 1",          {RGB15(28, 17, 15), RGB15(18, 11, 12), RGB15( 6,  1, 13), RGB15( 4,  4,  1)}},
    {"poolors 2",          {RGB15(22, 30, 29), RGB15(25, 22, 22), RGB15(21, 22, 15), RGB15( 8, 18, 17)}},
    {"poolors 3",          {RGB15(31, 27,  7), RGB15(18, 30, 21), RGB15(16, 11, 18), RGB15(10,  2, 13)}},
    {"poolors 4",          {RGB15(18, 25, 18), RGB15( 1, 18, 18), RGB15( 9,  9, 21), RGB15( 2,  1,  2)}}
};

// Values taken from the cartridge header
u8 ramSize;
u8 mapper;
u8 cgbFlag;
u8 romSize;
u8 romChecksum;
u16 romLicensee;

int keysPressed=0;
int lastKeysPressed=0;
int keysForceReleased=0;
int repeatStartTimer=0;
int repeatTimer=0;

bool advanceFrame=false;

u8* romSlot0;
u8* romSlot1;
int maxLoadedRomBanks;
int numLoadedRomBanks;
u8* romBankSlots = NULL; // Each 0x4000 bytes = one slot
int bankSlotIDs[MAX_ROM_BANKS]; // Keeps track of which bank occupies which slot
std::vector<int> lastBanksUsed;

bool suspendStateExists;

void initInput()
{
    //fatInit(FAT_CACHE_SIZE, true);
    fatInitDefault();

    maxLoadedRomBanks = (isDSiMode() ? 512 : 128);
    romBankSlots = (u8*)malloc(maxLoadedRomBanks*0x4000);
}

// This function is supposed to flush the cache so I don't have to fclose() and fopen()
// a file in order to save it.
// But I found I could not rely on it in the gameboySyncAutosave() function.
void flushFatCache() {
    // This involves things from libfat which aren't normally visible
    //devoptab_t* devops = (devoptab_t*)GetDeviceOpTab ("sd");
    //PARTITION* partition = (PARTITION*)devops->deviceData;
    //_FAT_cache_flush(partition->cache); // Flush the cache manually

    // BlocksDS always writes to the SD card, it doesn't keep blocks in the
    // cache
}

const char* gbKeyNames[] = {"-","A","B","Left","Right","Up","Down","Start","Select",
    "Menu","Menu/Pause","Save","Autofire A","Autofire B", "Fast Forward", "FF Toggle", "Scale","Reset","A+B+START+SELECT"};
const char* dsKeyNames[] = {"A","B","Select","Start","Right","Left","Up","Down",
    "R","L","X","Y","Touch"};

const int NUM_DS_KEYS = sizeof(dsKeyNames)/sizeof(char*);
const int NUM_GB_KEYS = sizeof(gbKeyNames)/sizeof(char*);

int keys[NUM_GB_KEYS];

struct KeyConfig {
    char name[32];
    int gbKeys[13];
};
KeyConfig defaultKeyConfig = {
    "Main",
    {KEY_GB_A,KEY_GB_B,KEY_GB_SELECT,KEY_GB_START,KEY_GB_RIGHT,KEY_GB_LEFT,KEY_GB_UP,KEY_GB_DOWN,
        KEY_MENU,KEY_FAST_FORWARD,KEY_SAVE,KEY_SCALE,KEY_MENU}
};

std::vector<KeyConfig> keyConfigs;
unsigned int selectedKeyConfig=0;

// Return false if there is no key assigned to opening the menu (including touch).
bool checkKeyAssignedToMenu(KeyConfig* config) {
    for (int i=0; i<NUM_DS_KEYS; i++) {
        if (config->gbKeys[i] == KEY_MENU || config->gbKeys[i] == KEY_MENU_PAUSE) {
            return true;
        }
    }

    return false;
}

void loadKeyConfig() {
    KeyConfig* keyConfig = &keyConfigs[selectedKeyConfig];
    for (int i=0; i<NUM_GB_KEYS; i++)
        keys[i] = 0;
    for (int i=0; i<NUM_DS_KEYS; i++) {
        keys[keyConfig->gbKeys[i]] |= BIT(i);
    }

    // Set "touch" to open the menu if nothing is assigned to the menu
    if (!checkKeyAssignedToMenu(keyConfig))
        keyConfig->gbKeys[12] = KEY_MENU;
}

void controlsParseConfig(const char* line2) {
    char line[100];
    strncpy(line, line2, 100);
    while (strlen(line) > 0 && (line[strlen(line)-1] == '\n' || line[strlen(line)-1] == ' '))
        line[strlen(line)-1] = '\0';
    if (line[0] == '(') {
        char* bracketEnd;
        if ((bracketEnd = strrchr(line, ')')) != 0) {
            *bracketEnd = '\0';
            const char* name = line+1;

            keyConfigs.push_back(KeyConfig());
            KeyConfig* config = &keyConfigs.back();
            strncpy(config->name, name, 32);
            config->name[31] = '\0';
            for (int i=0; i<NUM_DS_KEYS; i++)
                config->gbKeys[i] = KEY_NONE;
        }
        return;
    }
    char* equalsPos;
    if ((equalsPos = strrchr(line, '=')) != 0 && equalsPos != line+strlen(line)-1) {
        *equalsPos = '\0';

        if (strcasecmp(line, "config") == 0) {
            selectedKeyConfig = atoi(equalsPos+1);
        }
        else {
            int dsKey = -1;
            for (int i=0; i<NUM_DS_KEYS; i++) {
                if (strcasecmp(line, dsKeyNames[i]) == 0) {
                    dsKey = i;
                    break;
                }
            }
            int gbKey = -1;
            for (int i=0; i<NUM_GB_KEYS; i++) {
                if (strcasecmp(equalsPos+1, gbKeyNames[i]) == 0) {
                    gbKey = i;
                    break;
                }
            }

            if (gbKey != -1 && dsKey != -1) {
                KeyConfig* config = &keyConfigs.back();
                config->gbKeys[dsKey] = gbKey;
            }
        }
    }
}
void controlsPrintConfig(FILE* file) {
    fprintf(file, "config=%d\n", selectedKeyConfig);
    for (unsigned int i=0; i<keyConfigs.size(); i++) {
        fprintf(file, "(%s)\n", keyConfigs[i].name);
        for (int j=0; j<NUM_DS_KEYS; j++) {
            fprintf(file, "%s=%s\n", dsKeyNames[j], gbKeyNames[keyConfigs[i].gbKeys[j]]);
        }
    }
}

int keyConfigChooser_option;
bool keyConfigChooser_printMenuWarning = false;

void redrawKeyConfigChooser() {
    int& option = keyConfigChooser_option;
    KeyConfig* config = &keyConfigs[selectedKeyConfig];

    consoleClear();

    printf("Config: ");
    if (option == -1)
        iprintfColored(CONSOLE_COLOR_LIGHT_YELLOW, "* %s *\n\n", config->name);
    else
        printf("  %s  \n\n", config->name);

    printf("    Button   Function\n\n");

    for (int i=0; i<NUM_DS_KEYS; i++) {
        int len = 8-strlen(dsKeyNames[i]);
        while (len > 0) {
            printf(" ");
            len--;
        }
        if (option == i)
            iprintfColored(CONSOLE_COLOR_LIGHT_YELLOW, "* %s | %s *\n", dsKeyNames[i], gbKeyNames[config->gbKeys[i]]);
        else
            printf("  %s | %s  \n", dsKeyNames[i], gbKeyNames[config->gbKeys[i]]);
    }
    printf("\n\nPress X to make a new config.");
    if (selectedKeyConfig != 0) /* can't erase the default */ {
        printf("\n\nPress Y to delete this config.");
    }
    if (keyConfigChooser_printMenuWarning)
        printf("\n\nNo key is assigned to the menu!");
}

void updateKeyConfigChooser() {
    bool redraw = false;

    int& option = keyConfigChooser_option;
    KeyConfig* config = &keyConfigs[selectedKeyConfig];

    if (keyJustPressed(KEY_B)) {
        // Don't allow exiting if nothing is assigned to opening the menu

        if (!checkKeyAssignedToMenu(config)) {
            keyConfigChooser_printMenuWarning = true;
            redrawKeyConfigChooser();
        }
        else {
            loadKeyConfig();
            closeSubMenu();
        }
    }
    else if (keyJustPressed(KEY_X)) {
        keyConfigs.push_back(KeyConfig(*config));
        selectedKeyConfig = keyConfigs.size()-1;
        char name[32];
        sprintf(name, "Custom %d", keyConfigs.size()-1);
        strcpy(keyConfigs.back().name, name);
        option = -1;
        redraw = true;
    }
    else if (keyJustPressed(KEY_Y)) {
        if (selectedKeyConfig != 0) /* can't erase the default */ {
            keyConfigs.erase(keyConfigs.begin() + selectedKeyConfig);
            if (selectedKeyConfig >= keyConfigs.size())
                selectedKeyConfig = keyConfigs.size() - 1;
            redraw = true;
        }
    }
    else if (keyPressedAutoRepeat(KEY_DOWN)) {
        if (option == NUM_DS_KEYS-1)
            option = -1;
        else
            option++;
        redraw = true;
    }
    else if (keyPressedAutoRepeat(KEY_UP)) {
        if (option == -1)
            option = NUM_DS_KEYS-1;
        else
            option--;
        redraw = true;
    }
    else if (keyPressedAutoRepeat(KEY_LEFT)) {
        if (option == -1) {
            if (selectedKeyConfig == 0)
                selectedKeyConfig = keyConfigs.size()-1;
            else
                selectedKeyConfig--;
        }
        else {
            config->gbKeys[option]--;
            if (config->gbKeys[option] < 0)
                config->gbKeys[option] = NUM_GB_KEYS-1;
        }
        redraw = true;
    }
    else if (keyPressedAutoRepeat(KEY_RIGHT)) {
        if (option == -1) {
            selectedKeyConfig++;
            if (selectedKeyConfig >= keyConfigs.size())
                selectedKeyConfig = 0;
        }
        else {
            config->gbKeys[option]++;
            if (config->gbKeys[option] >= NUM_GB_KEYS)
                config->gbKeys[option] = 0;
        }
        redraw = true;
    }
    if (redraw)
        doAtVBlank(redrawKeyConfigChooser);
}

void startKeyConfigChooser() {
    keyConfigChooser_option = -1;
    displaySubMenu(updateKeyConfigChooser);
    redrawKeyConfigChooser();
}

int paletteChooser_option = 0;

void redrawPaletteChooser() {
    int& option = paletteChooser_option;

    int offset = -11 + option;

    consoleClear();

    // herein lies madness
    memset(&menuConsole->fontBgGfx[menuConsole->fontCharOffset + 1*16], 0x9999, sizeof(u16)*16);
    memset(&menuConsole->fontBgGfx[menuConsole->fontCharOffset + 2*16], 0xAAAA, sizeof(u16)*16);
    memset(&menuConsole->fontBgGfx[menuConsole->fontCharOffset + 3*16], 0xBBBB, sizeof(u16)*16);
    memset(&menuConsole->fontBgGfx[menuConsole->fontCharOffset + 4*16], 0xCCCC, sizeof(u16)*16);
    memset(&menuConsole->fontBgGfx[menuConsole->fontCharOffset + 5*16], 0xDDDD, sizeof(u16)*16);

    printf("%18s\n\n", (offset+2 > 0 ? "^^^^" : "    "));
    for(int i = 2; i < 22; i++) {
        int index = offset + i;
        int palIndex = (((i-2)%4))*64 + ((i-2)>>2) + 9;

        u16* dest = menuConsole->fontBgMap+i*32+2;

        if (index >= 0 && index < (int)customPalettes.size()) {
            int color = CONSOLE_COLOR_WHITE;
            if (index == option) {
                color = CONSOLE_COLOR_LIGHT_YELLOW;
            }
            iprintfColoredNoBreak(color, "%s      %s\n", (index == option ? "*" : " "), customPalettes[index].name);
            
            // palettes cycle around in unused space...
            BG_PALETTE_SUB[palIndex]    = customPalettes[index].palette[0];
            BG_PALETTE_SUB[palIndex+16] = customPalettes[index].palette[1];
            BG_PALETTE_SUB[palIndex+32] = customPalettes[index].palette[2];
            BG_PALETTE_SUB[palIndex+48] = customPalettes[index].palette[3];

            // ... for the cheeky palette preview tiles
            u8 tile = (((i-2)/4)%5)+1;
            *(dest++) = tile | TILE_PALETTE((((i-2)*4)%16));
            *(dest++) = tile | TILE_PALETTE((((i-2)*4)%16)+1);
            *(dest++) = tile | TILE_PALETTE((((i-2)*4)%16)+2);
            *(dest++) = tile | TILE_PALETTE((((i-2)*4)%16)+3);
        } else {
            printf("\n");
            BG_PALETTE_SUB[palIndex]    = 0;
            BG_PALETTE_SUB[palIndex+16] = 0;
            BG_PALETTE_SUB[palIndex+32] = 0;
            BG_PALETTE_SUB[palIndex+48] = 0;
        }
    }

    printf("\n%18s", (offset+22 < (int)customPalettes.size() ? "vvvv" : "    "));
}

void updatePaletteChooser() {
    bool redraw = false;

    int& option = paletteChooser_option;

    if (keyJustPressed(KEY_B)) {
        // just close
        closeSubMenu();
    }
    else if (keyJustPressed(KEY_A)) {
        // select palette and close
        customPalette = option;
        initGFXPalette(false);
        closeSubMenu();
    }
    else if (keyPressedAutoRepeat(KEY_DOWN)) {
        if (option == (int)customPalettes.size()-1)
            option = 0;
        else
            option++;
        redraw = true;
    }
    else if (keyPressedAutoRepeat(KEY_UP)) {
        if (option == 0)
            option = (int)customPalettes.size()-1;
        else
            option--;
        redraw = true;
    }
    else if (keyPressedAutoRepeat(KEY_LEFT)) {
        option -= 10;
        if (option < 0) option = 0;
        redraw = true;
    }
    else if (keyPressedAutoRepeat(KEY_RIGHT)) {
        option += 10;
        if (option >= (int)customPalettes.size()) option = (int)customPalettes.size()-1;
        redraw = true;
    }
    if (redraw)
        doAtVBlank(redrawPaletteChooser);
}

void startPaletteChooser() {
    paletteChooser_option = customPalette;
    displaySubMenu(updatePaletteChooser);
    redrawPaletteChooser();
}

void generalParseConfig(const char* line) {
    char* equalsPos;
    if ((equalsPos = strrchr(line, '=')) != 0 && equalsPos != line+strlen(line)-1) {
        *equalsPos = '\0';
        const char* parameter = line;
        const char* value = equalsPos+1;

        if (strcasecmp(parameter, "rompath") == 0) {
            if (romPath != 0)
                free(romPath);
            romPath = (char*)malloc(strlen(value)+1);
            strcpy(romPath, value);
            romChooserState.directory = romPath;
        }
        else if (strcasecmp(parameter, "biosfile") == 0) {
            if (biosPath != 0)
                free(biosPath);
            biosPath = (char*)malloc(strlen(value)+1);
            strcpy(biosPath, value);
            loadBios(biosPath);
        }
        else if (strcasecmp(parameter, "borderfile") == 0) {
            if (borderPath != 0)
                free(borderPath);
            borderPath = (char*)malloc(strlen(value)+1);
            strcpy(borderPath, value);
        }
    }
    if (borderPath == NULL || *borderPath == '\0') {
        free(borderPath);
        borderPath = (char*)malloc(strlen("/border.bmp")+1);
        strcpy(borderPath, "/border.bmp");
    }
}

void generalPrintConfig(FILE* file) {
    if (romPath == 0)
        fprintf(file, "rompath=\n");
    else
        fprintf(file, "rompath=%s\n", romPath);
    if (biosPath == 0)
        fprintf(file, "biosfile=\n");
    else
        fprintf(file, "biosfile=%s\n", biosPath);
    if (borderPath == 0)
        fprintf(file, "borderfile=\n");
    else
        fprintf(file, "borderfile=%s\n", borderPath);
}

bool RGBStringToPalette(const char* RGBString, u16* palette) {
    char lRGBString[40];
    strcpy(lRGBString, RGBString);
    const char sep[] = ",";
    char* token = strtok(lRGBString, sep);
    u8 rgbs[3];
    for (int i = 0; i < 4; i++) {
        for (int n = 0; n < 3; n++) {
            if (token == NULL) {
                // not enough RGB values, load default palette instead
                palette[0] = RGB15(31, 31, 31);
                palette[1] = RGB15(20, 20, 20);
                palette[2] = RGB15(10, 10, 10);
                palette[3] = RGB15( 0,  0,  0);
                return false;
            }
            int in = atoi(token);
            rgbs[n] = (u8)in;
            token = strtok(NULL, sep);
        }
        palette[i] = RGB15(rgbs[0], rgbs[1], rgbs[2]);
    }
    return true;
}

void paletteToRGBString(u16* palette, char* RGBString) {
    u8 rgbs[12];

    for (int i = 0; i < 4; i++) {
        rgbs[i*3]   = (palette[i])       & 0x1f;
        rgbs[i*3+1] = (palette[i] >> 5)  & 0x1f;
        rgbs[i*3+2] = (palette[i] >> 10) & 0x1f;
    }

    sprintf(RGBString, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
        rgbs[ 0], rgbs[ 1], rgbs[ 2],
        rgbs[ 3], rgbs[ 4], rgbs[ 5],
        rgbs[ 6], rgbs[ 7], rgbs[ 8],
        rgbs[ 9], rgbs[10], rgbs[11]);
}

void palettesParseConfig(const char* line) {
    char* equalsPos;
    if ((equalsPos = strrchr(line, '=')) != 0 && equalsPos != line+strlen(line)-1) {
        *equalsPos = '\0';
        const char* parameter = line;
        const char* value = equalsPos+1;

        PaletteEntry pal;
        strcpy(pal.name, parameter);
        if (RGBStringToPalette(value, pal.palette)) {
            customPalettes.push_back(pal);
        }
    }
}

void palettesPrintConfig(FILE* file) {
    for (PaletteEntry pal : customPalettes) {
        char RGBString[40];
        paletteToRGBString(pal.palette, RGBString);
        fprintf(file, "%s=%s\n", pal.name, RGBString);
    }
}

bool readConfigFile() {
    FILE* file = fopen(CONFIG_FILENAME, "r");
    char line[100];
    void (*configParser)(const char*) = generalParseConfig;

    if (file == NULL)
        goto end;

    while (!feof(file)) {
        fgets(line, 100, file);
        char c=0;
        while (*line != '\0' && ((c = line[strlen(line)-1]) == ' ' || c == '\n' || c == '\r'))
            line[strlen(line)-1] = '\0';
        if (line[0] == '[') {
            char* endBrace;
            if ((endBrace = strrchr(line, ']')) != 0) {
                *endBrace = '\0';
                const char* section = line+1;
                if (strcasecmp(section, "general") == 0) {
                    configParser = generalParseConfig;
                }
                else if (strcasecmp(section, "console") == 0) {
                    configParser = menuParseConfig;
                }
                else if (strcasecmp(section, "controls") == 0) {
                    configParser = controlsParseConfig;
                }
                else if (strcasecmp(section, "palettes") == 0) {
                    configParser = palettesParseConfig;
                }
            }
        }
        else
            configParser(line);
    }
    fclose(file);
end:
    if (keyConfigs.empty())
        keyConfigs.push_back(defaultKeyConfig);
    if (selectedKeyConfig >= keyConfigs.size())
        selectedKeyConfig = 0;
    loadKeyConfig();

    if (customPalettes.empty())
        customPalettes = defaultPalettes;

    if (customPalette < 0 || customPalette >= (int)customPalettes.size())
        customPalette = 0;

    return file != NULL;
}

bool writeConfigFile() {
    FILE* file = fopen(CONFIG_FILENAME, "w");
    if (!file)
        return false;

    fprintf(file, "[general]\n");
    generalPrintConfig(file);
    fprintf(file, "[console]\n");
    menuPrintConfig(file);
    fprintf(file, "[controls]\n");
    controlsPrintConfig(file);
    fprintf(file, "[palettes]\n");
    palettesPrintConfig(file);
    fclose(file);

    char nameBuf[100];
    sprintf(nameBuf, "%s.cht", basename);
    saveCheats(nameBuf);
    return true;
}


void loadBios(const char* filename) {
    FILE* file = fopen(filename, "rb");
    biosExists = file != NULL;
    if (biosExists) {
        fread(bios, 1, 0x900, file);
        fclose(file);
    }
}

int loadRom(char* f)
{
    if (romFile != NULL)
        fclose(romFile);
    strcpy(filename, f);

    // Check if this is a GBS file
    gbsMode = (strcasecmp(strrchr(filename, '.'), ".gbs") == 0);

    romFile = fopen(filename, "rb");
    if (romFile == NULL)
    {
        printLog("Error opening %s.\n", filename);
        return 1;
    }

    if (gbsMode) {
        fread(gbsHeader, 1, 0x70, romFile);
        gbsReadHeader();
        fseek(romFile, 0, SEEK_END);
        numRomBanks = (ftell(romFile)-0x70+0x3fff)/0x4000; // Get number of banks, rounded up
    }
    else {
        fseek(romFile, 0, SEEK_END);
        numRomBanks = (ftell(romFile)+0x3fff)/0x4000; // Get number of banks, rounded up
    }

    // Round numRomBanks to a power of 2
    int n=1;
    while (n < numRomBanks) n*=2;
    numRomBanks = n;

    //int rawRomSize = ftell(romFile);
    rewind(romFile);

    numLoadedRomBanks = std::min(numRomBanks,maxLoadedRomBanks);

    romSlot0 = romBankSlots;
    romSlot1 = romBankSlots + 0x4000;

    for (int i=0; i<numRomBanks; i++) {
        bankSlotIDs[i] = -1;
    }

    // Load rom banks and initialize all those "bank" arrays
    lastBanksUsed = std::vector<int>();
    // Read bank 0
    if (gbsMode) {
        bankSlotIDs[0] = 0;
        fseek(romFile, 0x70, SEEK_SET);
        fread(romBankSlots+gbsLoadAddress, 1, 0x4000-gbsLoadAddress, romFile);
    }
    else {
        bankSlotIDs[0] = 0;
        fseek(romFile, 0, SEEK_SET);
        fread(romBankSlots, 1, 0x4000, romFile);
    }
    // Read the rest of the banks
    for (int i=1; i<numLoadedRomBanks; i++) {
        bankSlotIDs[i] = i;
        fread(romBankSlots+0x4000*i, 1, 0x4000, romFile);
        lastBanksUsed.push_back(i);
    }

    strcpy(basename, filename);
    *(strrchr(basename, '.')) = '\0';
    strcpy(savename, basename);
    strcat(savename, ".sav");

    cgbFlag = romSlot0[0x143];
    romSize = romSlot0[0x148];
    ramSize = romSlot0[0x149];
    mapper  = romSlot0[0x147];
    romLicensee = romSlot0[0x14B];
    if (romLicensee == 0x33)
        romLicensee = romSlot0[0x144]<<8 | romSlot0[0x145];
    romChecksum = 0;
    for (int i=0; i<16; i++)
        romChecksum += romSlot0[i+0x134];

    int nameLength = 16;
    if (cgbFlag == 0x80 || cgbFlag == 0xc0)
        nameLength = 15;
    for (int i=0; i<nameLength; i++)
        romTitle[i] = (char)romSlot0[i+0x134];
    romTitle[nameLength] = '\0';

    hasRumble = false;

    if (gbsMode) {
        MBC = MBC5;
        loadCheats(""); // Unloads previous cheats
    } else {
        switch (mapper) {
            case 0: case 8: case 9:
                MBC = MBC0;
                break;
            case 1: case 2: case 3:
                MBC = MBC1;
                break;
            case 5: case 6:
                MBC = MBC2;
                break;
                //case 0xb: case 0xc: case 0xd:
                //MBC = MMM01;
                //break;
            case 0xf: case 0x10: case 0x11: case 0x12: case 0x13:
                MBC = MBC3;
                break;
                //case 0x15: case 0x16: case 0x17:
                //MBC = MBC4;
                //break;
            case 0x19: case 0x1a: case 0x1b:
                MBC = MBC5;
                break;
            case 0x1c: case 0x1d: case 0x1e:
                MBC = MBC5;
                hasRumble = true;
                break;
            case 0x22:
                MBC = MBC7;
                break;
            case 0xea: /* Hack for SONIC5 */
                MBC = MBC1;
                break;
            case 0xfc:  /* Game Boy Camera */
                MBC = POCKET_CAM;
                break;
            case 0xfe:
                MBC = HUC3;
                break;
            case 0xff:
                MBC = HUC1;
                break;
            default:
                printLog("Unsupported MBC %02x\n", mapper);
                return 1;
        }

        // Little hack to preserve "quickread" from gbcpu.cpp.
        if (biosExists) {
            for (int i=0x100; i<0x150; i++)
                bios[i] = romSlot0[i];
        }

        suspendStateExists = checkStateExists(-1);

        // Load cheats
        char nameBuf[100];
        sprintf(nameBuf, "%s.cht", basename);
        loadCheats(nameBuf);

    } // !gbsMode

    // If we've loaded everything, close the rom file
    if (numRomBanks <= numLoadedRomBanks) {
        fclose(romFile);
        romFile = NULL;
    }

    loadSave();

    return 0;
}

void loadRomBank() {
    if (bankSlotIDs[romBank] != -1 || numRomBanks <= numLoadedRomBanks || romBank == 0) {
        romSlot1 = romBankSlots+bankSlotIDs[romBank]*0x4000;
        return;
    }
    int bankToUnload = lastBanksUsed.back();
    lastBanksUsed.pop_back();
    int slot = bankSlotIDs[bankToUnload];
    bankSlotIDs[bankToUnload] = -1;
    bankSlotIDs[romBank] = slot;

    fseek(romFile, 0x4000*romBank, SEEK_SET);
    fread(romBankSlots+slot*0x4000, 1, 0x4000, romFile);

    lastBanksUsed.insert(lastBanksUsed.begin(), romBank);

    applyGGCheatsToBank(romBank);

    romSlot1 = romBankSlots+slot*0x4000;
}

bool isRomBankLoaded(int bank) {
    return bankSlotIDs[bank] != -1;
}
u8* getRomBank(int bank) {
    if (!isRomBankLoaded(bank))
        return 0;
    return romBankSlots+bankSlotIDs[bank]*0x4000;
}

const char* getRomBasename() {
    return basename;
}

void unloadRom() {
    doAtVBlank(clearGFX);

    gameboySyncAutosave();
    if (saveFile != NULL)
        fclose(saveFile);
    saveFile = NULL;
    // unload previous save
    if (externRam != NULL) {
        free(externRam);
        externRam = NULL;
    }
}

int loadSave()
{
    if (saveFile != NULL) {
        fclose(saveFile);
        saveFile = NULL;
    }
    if (externRam != NULL) {
        free(externRam);
        externRam = NULL;
    }

    if (gbsMode)
        numRamBanks = 1;
    else {
        // Get the game's external memory size and allocate the memory
        switch(ramSize)
        {
            case 0:
                numRamBanks = 0;
                break;
            case 1:
            case 2:
                numRamBanks = 1;
                break;
            case 3:
                numRamBanks = 4;
                break;
            case 4:
                numRamBanks = 16;
                break;
            default:
                printLog("Invalid RAM bank number: %x\nDefaulting to 4 banks\n", ramSize);
                numRamBanks = 4;
                break;
        }
        if (MBC == MBC2)
            numRamBanks = 1;
    }

    if (numRamBanks == 0)
        return 0;

    externRam = (u8*)malloc(numRamBanks*0x2000);

    if (gbsMode)
        return 0; // GBS files don't get to save.

    // Now load the data.
    saveFile = fopen(savename, "r+b");
    int neededFileSize = numRamBanks*0x2000;
    if (MBC == MBC3 || MBC == HUC3)
        neededFileSize += sizeof(clockStruct);

    int fileSize = 0;
    if (saveFile) {
        fseek(saveFile, 0, SEEK_END);
        fileSize = ftell(saveFile);
        fseek(saveFile, 0, SEEK_SET);
    }

    if (!saveFile || fileSize < neededFileSize) {
        fclose(saveFile);

        // Extend the size of the file, or create it
        if (!saveFile) {
            saveFile = fopen(savename, "wb");
            fseek(saveFile, neededFileSize-1, SEEK_SET);
            fputc(0, saveFile);
        }
        else {
            saveFile = fopen(savename, "ab");
            for (; fileSize<neededFileSize; fileSize++)
                fputc(0, saveFile);
        }
        fclose(saveFile);

        saveFile = fopen(savename, "r+b");
    }

    fread(externRam, 1, 0x2000*numRamBanks, saveFile);

    switch (MBC) {
        case MBC3:
        case HUC3:
            fread(&gbClock, 1, sizeof(gbClock), saveFile);
            break;
    }

    return 0;
}

int saveGame()
{
    if (numRamBanks == 0 || saveFile == NULL)
        return 0;

    printLog("Full game save\n");

    fseek(saveFile, 0, SEEK_SET);

    fwrite(externRam, 1, 0x2000*numRamBanks, saveFile);

    switch (MBC) {
        case MBC3:
        case HUC3:
            fwrite(&gbClock, 1, sizeof(gbClock), saveFile);
            break;
    }

    flushFatCache();

    return 0;
}

bool wroteToSramThisFrame=false;
int framesSinceAutosaveStarted=0;

void gameboySyncAutosave() {
    if (!autosaveStarted)
        return;

    numSaveWrites = 0;
    wroteToSramThisFrame = false;

    int numSectors = 0;
    int lastWritten = -2;

    // iterate over each sector
    for (int i=0; i<numRamBanks*0x2000/AUTOSAVE_SECTOR_SIZE; i++) {
        if (dirtySectors[i]) {

            if (lastWritten+1 != i)
                fseek(saveFile, i*AUTOSAVE_SECTOR_SIZE, SEEK_SET);

            fwrite(externRam+i*AUTOSAVE_SECTOR_SIZE, AUTOSAVE_SECTOR_SIZE, 1, saveFile);

            lastWritten = i;
            dirtySectors[i] = false;
            numSectors++;
        }
    }
    printLog("SAVE %d sectors\n", numSectors);

    fclose(saveFile);
    saveFile = fopen(savename, "r+b");

    framesSinceAutosaveStarted = 0;
    autosaveStarted = false;
}

void updateAutosave() {
    if (autosaveStarted)
        framesSinceAutosaveStarted++;

    if (framesSinceAutosaveStarted >= 120 ||     // Executes when sram is written to for 120 consecutive frames, or
        (!saveModified && wroteToSramThisFrame)) { // when a full frame has passed since sram was last written to.
        gameboySyncAutosave();
    }
    if (saveModified) {
        wroteToSramThisFrame = true;
        autosaveStarted = true;
        saveModified = false;
    }
}


bool keyPressed(int key) {
    return keysPressed&key;
}
bool keyPressedAutoRepeat(int key) {
    if (keyJustPressed(key)) {
        repeatStartTimer = 14;
        return true;
    }
    if (keyPressed(key) && repeatStartTimer == 0 && repeatTimer == 0) {
        repeatTimer = 2;
        return true;
    }
    return false;
}
bool keyJustPressed(int key) {
    return ((keysPressed^lastKeysPressed)&keysPressed) & key;
}

int readKeysLastFrameCounter=0;
void readKeys() {
    scanKeys();

    lastKeysPressed = keysPressed;
    keysPressed = keysHeld();
    for (int i=0; i<16; i++) {
        if (keysForceReleased & (1<<i)) {
            if (!(keysPressed & (1<<i)))
                keysForceReleased &= ~(1<<i);
        }
    }
    keysPressed &= ~keysForceReleased;

    if (dsFrameCounter != readKeysLastFrameCounter) { // Double-check that it's been 1/60th of a second
        if (repeatStartTimer > 0)
            repeatStartTimer--;
        if (repeatTimer > 0)
            repeatTimer--;
        readKeysLastFrameCounter = dsFrameCounter;
    }
}

void forceReleaseKey(int key) {
    keysForceReleased |= key;
    keysPressed &= ~key;
}

int mapGbKey(int gbKey) {
    return keys[gbKey];
}

char* getRomTitle() {
    return romTitle;
}

const char *mbcName[] = {"ROM","MBC1","MBC2","MBC3","MBC4","MBC5","MBC7","HUC3","HUC1","POCKET_CAM"};

void printRomInfo() {
    consoleClear();
    printf("ROM Title: \"%s\"\n", romTitle);
    printf("Cartridge type: %.2x (%s)\n", mapper, mbcName[MBC]);
    printf("ROM Size: %.2x (%d banks)\n", romSize, numRomBanks);
    printf("RAM Size: %.2x (%d banks)\n", ramSize, numRamBanks);
    printf("Licensee: %04X\n", romLicensee);
    printf("Checksum: %02X\n", romChecksum);
}

const int STATE_VERSION = 5;

struct StateStruct {
    // version
    // bg/sprite PaletteData
    // vram
    // wram
    // hram
    // sram
    Registers regs;
    int halt, ime;
    bool doubleSpeed, biosOn;
    int gbMode;
    int romBank, ramBank, wramBank, vramBank;
    int memoryModel;
    clockStruct clock;
    int scanlineCounter, timerCounter, phaseCounter, dividerCounter;
    // v2
    int serialCounter;
    // v3
    bool ramEnabled;
    // MBC-specific stuff
    // v4
    //  bool sgbMode;
    //  If sgbMode == true:
    //   int sgbPacketLength;
    //   int sgbPacketsTransferred;
    //   int sgbPacketBit;
    //   u8 sgbCommand;
    //   u8 gfxMask;
    //   u8[20*18] sgbMap;
};

void saveState(int stateNum) {
    FILE* outFile;
    StateStruct state;
    char statename[100];

    if (stateNum == -1)
        sprintf(statename, "%s.yss", basename);
    else
        sprintf(statename, "%s.ys%d", basename, stateNum);
    outFile = fopen(statename, "w");

    if (outFile == 0) {
        printMenuMessage("Error opening file for writing.");
        return;
    }

    state.regs = gbRegs;
    state.halt = halt;
    state.ime = ime;
    state.doubleSpeed = doubleSpeed;
    state.biosOn = biosOn;
    state.gbMode = gbMode;
    state.romBank = romBank;
    state.ramBank = currentRamBank;
    state.wramBank = wramBank;
    state.vramBank = vramBank;
    state.memoryModel = memoryModel;
    state.clock = gbClock;
    state.scanlineCounter = scanlineCounter;
    state.timerCounter = timerCounter;
    state.phaseCounter = phaseCounter;
    state.dividerCounter = dividerCounter;
    state.serialCounter = serialCounter;
    state.ramEnabled = ramEnabled;

    fwrite(&STATE_VERSION, sizeof(int), 1, outFile);
    fwrite((char*)bgPaletteData, 1, sizeof(bgPaletteData), outFile);
    fwrite((char*)sprPaletteData, 1, sizeof(sprPaletteData), outFile);
    fwrite((char*)vram, 1, sizeof(vram), outFile);
    fwrite((char*)wram, 1, sizeof(wram), outFile);
    fwrite((char*)hram, 1, 0x200, outFile);
    fwrite((char*)externRam, 1, 0x2000*numRamBanks, outFile);

    fwrite((char*)&state, 1, sizeof(StateStruct), outFile);

    switch (MBC) {
        case HUC3:
            fwrite(&HuC3Mode,  1, sizeof(u8), outFile);
            fwrite(&HuC3Value, 1, sizeof(u8), outFile);
            fwrite(&HuC3Shift, 1, sizeof(u8), outFile);
            break;
    }

    fwrite(&sgbMode, 1, sizeof(bool), outFile);
    if (sgbMode) {
        fwrite(&sgbPacketLength, 1, sizeof(int), outFile);
        fwrite(&sgbPacketsTransferred, 1, sizeof(int), outFile);
        fwrite(&sgbPacketBit, 1, sizeof(int), outFile);
        fwrite(&sgbCommand, 1, sizeof(u8), outFile);
        fwrite(&gfxMask, 1, sizeof(u8), outFile);
        fwrite(sgbMap, 1, sizeof(sgbMap), outFile);
    }

    fclose(outFile);
}

int loadState(int stateNum) {
    FILE *inFile;
    StateStruct state;
    char statename[100];
    int version;

    memset(&state, 0, sizeof(StateStruct));

    if (stateNum == -1)
        sprintf(statename, "%s.yss", basename);
    else
        sprintf(statename, "%s.ys%d", basename, stateNum);
    inFile = fopen(statename, "r");

    if (inFile == 0) {
        printMenuMessage("State doesn't exist.");
        return 1;
    }

    fread(&version, sizeof(int), 1, inFile);

    if (version == 0 || version > STATE_VERSION) {
        printMenuMessage("State is from an incompatible version.");
        return 1;
    }

    fread((char*)bgPaletteData, 1, sizeof(bgPaletteData), inFile);
    fread((char*)sprPaletteData, 1, sizeof(sprPaletteData), inFile);
    fread((char*)vram, 1, sizeof(vram), inFile);
    fread((char*)wram, 1, sizeof(wram), inFile);
    fread((char*)hram, 1, 0x200, inFile);

    if (version <= 4 && ramSize == 0x04)
        // Value "0x04" for ram size wasn't interpreted correctly before
        fread((char*)externRam, 1, 0x2000*4, inFile);
    else
        fread((char*)externRam, 1, 0x2000*numRamBanks, inFile);

    fread((char*)&state, 1, sizeof(StateStruct), inFile);

    /* MBC-specific values have been introduced in v3 */
    if (version >= 3) {
        switch (MBC) {
            case MBC3:
                if (version == 3) {
                    u8 rtcReg;
                    fread(&rtcReg, 1, sizeof(u8), inFile);
                    if (rtcReg != 0)
                        currentRamBank = rtcReg;
                }
                break;
            case HUC3:
                fread(&HuC3Mode,  1, sizeof(u8), inFile);
                fread(&HuC3Value, 1, sizeof(u8), inFile);
                fread(&HuC3Shift, 1, sizeof(u8), inFile);
                break;
        }

        fread(&sgbMode, 1, sizeof(bool), inFile);
        if (sgbMode) {
            fread(&sgbPacketLength, 1, sizeof(int), inFile);
            fread(&sgbPacketsTransferred, 1, sizeof(int), inFile);
            fread(&sgbPacketBit, 1, sizeof(int), inFile);
            fread(&sgbCommand, 1, sizeof(u8), inFile);
            fread(&gfxMask, 1, sizeof(u8), inFile);
            fread(sgbMap, 1, sizeof(sgbMap), inFile);
        }
    }
    else
        sgbMode = false;


    fclose(inFile);
    if (stateNum == -1) {
        unlink(statename);
        suspendStateExists = false;
    }

    gbRegs = state.regs;
    halt = state.halt;
    ime = state.ime;
    doubleSpeed = state.doubleSpeed;
    biosOn = state.biosOn;
    if (!biosExists)
        biosOn = false;
    gbMode = state.gbMode;
    romBank = state.romBank;
    currentRamBank = state.ramBank;
    wramBank = state.wramBank;
    vramBank = state.vramBank;
    memoryModel = state.memoryModel;
    gbClock = state.clock;
    scanlineCounter = state.scanlineCounter;
    timerCounter = state.timerCounter;
    phaseCounter = state.phaseCounter;
    dividerCounter = state.dividerCounter;
    serialCounter = state.serialCounter;
    ramEnabled = state.ramEnabled;
    if (version < 3)
        ramEnabled = true;

    transferReady = false;
    timerPeriod = periods[ioRam[0x07]&0x3];
    cyclesToEvent = 1;

    mapMemory();
    setDoubleSpeed(doubleSpeed);


    if (autoSavingEnabled && stateNum != -1)
        saveGame(); // Synchronize save file on sd with file in ram

    refreshGFX();
    refreshSND();

    return 0;
}

void deleteState(int stateNum) {
    if (!checkStateExists(stateNum))
        return;

    char statename[100];

    if (stateNum == -1)
        sprintf(statename, "%s.yss", basename);
    else
        sprintf(statename, "%s.ys%d", basename, stateNum);
    unlink(statename);
}

bool checkStateExists(int stateNum) {
    char statename[256];

    if (stateNum == -1)
        sprintf(statename, "%s.yss", basename);
    else
        sprintf(statename, "%s.ys%d", basename, stateNum);
    return access(statename, R_OK) == 0;
    /*
    file = fopen(statename, "r");

    if (file == 0) {
        return false;
    }
    fclose(file);
    return true;
    */
}


