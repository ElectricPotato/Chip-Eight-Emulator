
#include <stdio.h>
#include <stdlib.h> // system/malloc/free
#include <stdint.h>
#include <stdbool.h>

#include <string.h> //for memcpy

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

//32*64/8 = 256
#define GRAPHICS_MEMORY_SIZE (SCREEN_WIDTH*SCREEN_HEIGHT/8)

uint8_t graphics_memory[GRAPHICS_MEMORY_SIZE];


void clear_screen(void);
bool flip_pixel(int8_t, int8_t, bool);
bool render_sprite(uint8_t, uint8_t, uint8_t);

void draw_screen_ascii(void);

void draw_sprite_ascii(uint8_t);