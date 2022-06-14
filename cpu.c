#include "cpu.h"
#include "registers.h"
#include "graphics.h"
#include "debug.h"


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

#define REG_VX reg_bank[INSTR_X]
#define REG_VY reg_bank[INSTR_Y]
//Flag register
#define REG_VF reg_bank[0xF]


void reset_cpu(void){
    I_reg = 0;

    stack_size = 0;

    timer_delay = 0;
    timer_sound = 0;

    memset(stack, 0, sizeof(stack)); //stack for easier debugging

    memset(reg_bank, 0, sizeof(reg_bank));
}

//one cpu cycle
void cycle(void){
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

