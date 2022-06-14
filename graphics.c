
#include <stdio.h>
#include <stdlib.h> // system/malloc/free
#include <stdint.h>
#include <stdbool.h>

#include <string.h> //for memcpy

#include "registers.h"
#include "graphics.h"

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

bool flip_pixel(int8_t x, int8_t y, bool flip){
    uint16_t bit_n = y * SCREEN_WIDTH + x; //calculate the position of the bit in graphics memory
    uint8_t bitmask = flip << (7 - (bit_n % 8)); //calculate the position of the bit to flip within a byte corresponding to the specific pixel

    bool collision = (graphics_memory[bit_n / 8] & bitmask) != 0; //check for pixel being turned off
    graphics_memory[bit_n / 8] ^= bitmask; //flip pixel

    return collision;
}

//debug functions

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