#include <nds/arm9/console.h>
#include <stdio.h>

void printVersionInfo() {
    consoleClear();
    printf("GameYob %s\n", "VERSION_STRING");
}
