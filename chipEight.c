
#include <stdio.h>
#include <stdlib.h> // system/malloc/free
#include <stdint.h>
#include <stdbool.h>

#include <string.h> //for memcpy


//Guide used: https://tobiasvl.github.io/blog/write-a-chip-8-emulator/


#define DEBUG 1
#include "debug.h"

#include "font.h"
#include "ibm_logo_rom.h"

#define FONT_BASE_ADDR 0x050
#define PROGRAM_BASE_ADDRESS 0x200


//4kB of memory
#define MEM_SIZE 4096

uint8_t memory[MEM_SIZE];

uint16_t PC = PROGRAM_BASE_ADDRESS; //program counter (only 12 bits are used to address all of the 4096 bytes of memory)
uint16_t I_reg = 0; //16-bit index register called “I” which is used to point at locations in memory (only 12 bits used)

//stack for 16-bit addresses, which is used to call subroutines/functions and return from them
#define MAX_STACK_SIZE 255

uint16_t stack_size = 0;
uint16_t stack[MAX_STACK_SIZE];
//TODO: make stack dynamically allocated

uint8_t timer_delay = 0; //decremented at a rate of 60 Hz (60 times per second) until it reaches 0
uint8_t timer_sound = 0; //like the delay timer, but which also gives off a beeping sound as long as it’s not 0

uint8_t reg_bank[16]; //16 8-bit general-purpose variable registers numbered 0 through F hexadecimal, ie. 0 through 15 in decimal, called V0 through VF
//VF is also used as a flag register; many instructions will set it to either 1 or 0 based on some rule, for example using it as a carry flag

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

//32*64/8 = 256
#define GRAPHICS_MEMORY_SIZE (SCREEN_WIDTH*SCREEN_HEIGHT/8)

uint8_t graphics_memory[GRAPHICS_MEMORY_SIZE];

void clear_screen(void){
    memset(graphics_memory, 0, GRAPHICS_MEMORY_SIZE);
}

void draw_screen_ascii(void){
    //MSb first, LSB first - highest significance bit in a byte is the left most, index 0 byte is the leftmost
    //|byte 0||byte 1||byte 2||byte 3|
    //76543210765432107654321076543210

    uint16_t bit_n = 0;
    for(int screen_y = 0; screen_y < SCREEN_HEIGHT; screen_y++){
        for(int screen_x = 0; screen_x < SCREEN_WIDTH; screen_x++){
            //if((graphics_memory[bit_n / 8] << (bit_n % 8)) & 0b1000000){
            if((graphics_memory[bit_n / 8] >> (7 - (bit_n % 8))) & 0b1){
                printf("*");
            }else{
                printf(" ");
            }
            bit_n++;
        }
        printf("\n");
    }
    printf("---\n");

}

void draw_sprite_ascii(uint8_t N){
    //MSb first, LSB first - highest significance bit in a byte is the left most, index 0 byte is the leftmost
    //|byte 0||byte 1||byte 2||byte 3|
    //76543210765432107654321076543210

    for(int screen_y = 0; screen_y < N; screen_y++){
        for(int screen_x = 0; screen_x < 8; screen_x++){
            if((memory[I_reg + screen_y] >> (7 - screen_x)) & 0x1){
                printf("*");
            }else{
                printf(" ");
            }
        }
        printf("\n");
    }
    printf("---\n");

}

bool flip_pixel(int8_t x, int8_t y, bool flip){
    uint16_t bit_n = y * SCREEN_WIDTH + x; //calculate the position of the bit in graphics memory
    uint8_t bitmask = flip << (7 - (bit_n % 8)); //calculate the position of the bit to flip within a byte corresponding to the specific pixel

    bool collision = (graphics_memory[bit_n / 8] & bitmask) != 0; //check for pixel being turned off
    graphics_memory[bit_n / 8] ^= bitmask; //flip pixel

    return collision;
}

bool render_sprite(uint8_t start_x, uint8_t start_y, uint8_t n_lines){
    bool collision = false;
    for(int sprite_y = 0; (sprite_y < n_lines) && ((start_y + sprite_y) < SCREEN_HEIGHT); sprite_y++){
        for(int sprite_x = 0; (sprite_x < 8) && ((start_x + sprite_x) < SCREEN_WIDTH); sprite_x++){
            bool pixel = ((memory[I_reg + sprite_y] >> (7 - sprite_x)) & 0x1) != 0;
            collision &= flip_pixel(start_x + sprite_x, start_y + sprite_y, pixel);
        }
    }
    return collision;
}



void setup(void){
    memset(memory, 0, MEM_SIZE); //clear memory for easier debugging
    memcpy(memory + FONT_BASE_ADDR, font_data, sizeof(font_data)); //copy font data into memory
    memcpy(memory + PROGRAM_BASE_ADDRESS, PROGRAM, sizeof(PROGRAM)); //copy program into memory

    clear_screen();
    memset(reg_bank, 0, sizeof(reg_bank));
}


#define REG_VX reg_bank[INSTR_X]
#define REG_VY reg_bank[INSTR_Y]
//Flag register
#define REG_VF reg_bank[0xF]


// OPCODE 1st nibble,          4 bits
// X      2nd nibble,          4 bits
// Y      3rd nibble,          4 bits
// N      4th nibble,          4 bits
// NN     3rd/4th nibble,      8 bits
// NNN    2nd/3rd/4th nibble, 12 bits

#define INSTR_OPCODE (instr >> 12 &   0xF)
#define INSTR_X      (instr >>  8 &   0xF) 
#define INSTR_Y      (instr >>  4 &   0xF)
#define INSTR_N      (instr       &   0xF)
#define INSTR_NN     (instr       &  0xFF)
#define INSTR_NNN    (instr       & 0xFFF)



void loop(void){
    //fetch
    uint16_t instr = memory[PC] << 8 | memory[PC + 1];

    debug_print("PC:%x I_reg:%x\n", PC, I_reg);
    
    debug_print("Regs:");
    for(int i = 0; i < 16; i++){debug_print("%x ", reg_bank[i]);}
    debug_print("\n");
    
    debug_print("Instr:%x\n", instr);

    //execute
    if(instr == 0x00E0){ //00E0 (clear screen)
        debug_print("clear screen\n");

        clear_screen();
        PC += 2;
    }else if(INSTR_OPCODE == 0x1){ //1NNN (jump)
        debug_print("jump %x\n", INSTR_NNN);

        PC = INSTR_NNN;
    }else if(INSTR_OPCODE == 0x6){ //6XNN (set register VX)
        debug_print("V%x = %x\n", INSTR_X, INSTR_NN);

        REG_VX = INSTR_NN;
        PC += 2;
    }else if(INSTR_OPCODE == 0x7){ //7XNN (add value to register VX)
        debug_print("V%x += %x\n", INSTR_X, INSTR_NN);

        REG_VX += INSTR_NN;
        PC += 2;
    }else if(INSTR_OPCODE == 0xA){ //ANNN (set index register I)
        debug_print("I_reg = %x\n", INSTR_NNN);

        I_reg = INSTR_NNN;
        PC += 2;
    }else if(INSTR_OPCODE == 0xD){ //DXYN (display/draw)
        debug_print("draw %x %x %x\n", INSTR_X, INSTR_Y, INSTR_N);

        uint8_t start_x = REG_VX % SCREEN_WIDTH;
        uint8_t start_y = REG_VY % SCREEN_HEIGHT;

        if(0){ //turn on to draw the current sprite being rendered
            draw_sprite_ascii(INSTR_N);
        }

        

        /*
        N pixels tall sprite from the memory location in I_reg to the screen,
        X coordinate in VX and the Y coordinate in VY. All the pixels that are “on” in the sprite will
        flip the pixels on the screen that it is drawn to. If any pixels on the screen were turned “off” by this,
        the VF flag register is set to 1. Otherwise, it’s set to 0.

        sprite is 8 pixels wide and N pixels high, stored line by line

        (0,0) is top left
        */

        REG_VF = render_sprite(start_x, start_y, INSTR_N);


        draw_screen_ascii();
        getchar();

        PC += 2;
    }

    debug_print("\n");
}


void test1(void){
    //colour in the screen one pixel at a time
    for(int screen_y = 0; screen_y < SCREEN_HEIGHT; screen_y++){
        for(int screen_x = 0; screen_x < SCREEN_WIDTH; screen_x++){
            flip_pixel(screen_x, screen_y, true);
            draw_screen_ascii();
            getchar();
        }
    }
}


int main(int argc, char** argv){  
    printf("Hello World\n");
    debug_print_detail("debugging ON\n");

    setup();
    while(true){
        loop();
    }

    return 0;
}