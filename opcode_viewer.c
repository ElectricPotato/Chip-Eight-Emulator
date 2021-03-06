
#include <stdio.h>
#include <stdlib.h> // system/malloc/free
#include <stdint.h>
#include <stdbool.h>

#include <string.h> //for memcpy


const uint8_t IBM_LOGO_ROM[] = {
    0x00, 0xe0, 0xa2, 0x2a, 0x60, 0x0c, 0x61, 0x08, 0xd0, 0x1f, 0x70, 0x09, 0xa2, 0x39, 0xd0, 0x1f,
    0xa2, 0x48, 0x70, 0x08, 0xd0, 0x1f, 0x70, 0x04, 0xa2, 0x57, 0xd0, 0x1f, 0x70, 0x08, 0xa2, 0x66,
    0xd0, 0x1f, 0x70, 0x08, 0xa2, 0x75, 0xd0, 0x1f, 0x12, 0x28, 0xff, 0x00, 0xff, 0x00, 0x3c, 0x00,
    0x3c, 0x00, 0x3c, 0x00, 0x3c, 0x00, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0x00, 0x38, 0x00, 0x3f,
    0x00, 0x3f, 0x00, 0x38, 0x00, 0xff, 0x00, 0xff, 0x80, 0x00, 0xe0, 0x00, 0xe0, 0x00, 0x80, 0x00,
    0x80, 0x00, 0xe0, 0x00, 0xe0, 0x00, 0x80, 0xf8, 0x00, 0xfc, 0x00, 0x3e, 0x00, 0x3f, 0x00, 0x3b,
    0x00, 0x39, 0x00, 0xf8, 0x00, 0xf8, 0x03, 0x00, 0x07, 0x00, 0x0f, 0x00, 0xbf, 0x00, 0xfb, 0x00,
    0xf3, 0x00, 0xe3, 0x00, 0x43, 0xe0, 0x00, 0xe0, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80,
    0x00, 0xe0, 0x00, 0xe0
};

#define PROGRAM_BASE_ADDRESS 0x200

uint16_t PC = 0;

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
    uint16_t instr = IBM_LOGO_ROM[PC] << 8 | IBM_LOGO_ROM[PC + 1];

    //execute

    printf("%x: ", PC + PROGRAM_BASE_ADDRESS);
    if(instr == 0x00E0){ //00E0 (clear screen)
        printf("clear screen\n");
    }else if(INSTR_OPCODE == 0x1){ //1NNN (jump)
        printf("jump %x\n", INSTR_NNN);
    }else if(INSTR_OPCODE == 0x6){ //6XNN (set register VX)
        printf("V%x = %x\n", INSTR_X, INSTR_NN);
    }else if(INSTR_OPCODE == 0x7){ //7XNN (add value to register VX)
        printf("V%x += %x\n", INSTR_X, INSTR_NN);
    }else if(INSTR_OPCODE == 0xA){ //ANNN (set index register I)
        printf("I_reg = %x\n", INSTR_NNN);
    }else if(INSTR_OPCODE == 0xD){ //DXYN (display/draw)
        printf("draw %x %x %x\n", INSTR_X, INSTR_Y, INSTR_N);
    }
    PC += 2;
}

int main(int argc, char** argv){    
    printf("Hello World\n");

    while(PC < sizeof(IBM_LOGO_ROM)){
        loop();
    }

    return 0;
}