#include <stdint.h>

#ifndef REGISTERS_H
#define REGISTERS_H

#define FONT_BASE_ADDR 0x050
#define PROGRAM_BASE_ADDRESS 0x200


//4kB of memory
#define MEM_SIZE 4096

extern uint8_t memory[MEM_SIZE];

extern uint16_t PC; //program counter (only 12 bits are used to address all of the 4096 bytes of memory)
extern uint16_t I_reg; //16-bit index register called “I” which is used to point at locations in memory (only 12 bits used)

//stack for 16-bit addresses, which is used to call subroutines/functions and return from them
#define MAX_STACK_SIZE 255

extern uint16_t stack_size;
extern uint16_t stack[MAX_STACK_SIZE];
//TODO: make stack dynamically allocated

extern uint8_t timer_delay; //decremented at a rate of 60 Hz (60 times per second) until it reaches 0
extern uint8_t timer_sound; //like the delay timer, but which also gives off a beeping sound as long as it’s not 0

extern uint8_t reg_bank[16]; //16 8-bit general-purpose variable registers numbered 0 through F hexadecimal, ie. 0 through 15 in decimal, called V0 through VF
//VF is also used as a flag register; many instructions will set it to either 1 or 0 based on some rule, for example using it as a carry flag

#endif /* REGISTERS_H */





