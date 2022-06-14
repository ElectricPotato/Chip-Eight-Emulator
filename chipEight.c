
#include <stdio.h>
#include <stdlib.h> // system/malloc/free
#include <stdint.h>
#include <stdbool.h>

#include <string.h> //for memcpy


//Guide used: https://tobiasvl.github.io/blog/write-a-chip-8-emulator/

#include "debug.h"

#include "font.h"
#include "ibm_logo_rom.h"

#include "registers.h"
#include "graphics.h"
#include "cpu.h"



void setup(void){
    reset_cpu();
    clear_screen();

    //load memory
    memset(memory, 0, MEM_SIZE); //clear memory for easier debugging
    memcpy(memory + FONT_BASE_ADDR, font_data, sizeof(font_data)); //copy font data into memory
    memcpy(memory + PROGRAM_BASE_ADDRESS, PROGRAM, sizeof(PROGRAM)); //copy program into memory
    PC = PROGRAM_BASE_ADDRESS;
}






int main(int argc, char** argv){  
    printf("Hello World\n");
    debug_print_detail("debugging ON\n");

    setup();
    while(true){
        cycle();
    }

    return 0;
}