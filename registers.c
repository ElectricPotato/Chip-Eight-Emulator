#include "registers.h"


uint8_t memory[MEM_SIZE];

uint16_t PC = PROGRAM_BASE_ADDRESS;
uint16_t I_reg = 0;

uint16_t stack_size = 0;
uint16_t stack[MAX_STACK_SIZE];

uint8_t timer_delay = 0;
uint8_t timer_sound = 0;

uint8_t reg_bank[16];