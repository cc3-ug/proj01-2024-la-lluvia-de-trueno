#include "utils.h"
#include <stdio.h>
#include <stdlib.h>


//sign extends a bitfield with given size
/* You may find implementing this function helpful */
int bitExtender(unsigned int field, unsigned int size) {
  /* YOUR CODE HERE */
  int sign_bit = (field >> (size - 1)) & 0x1;
  int mask = (1 << size) - 1;

  if (sign_bit) {
    int sign_extension = ~((1 << size) - 1);
    return field | sign_extension;
  } else {
    return field & mask;
  }
}


/* Distances in BYTES */

int get_branch_distance(Instruction instruction) {
  /* YOUR CODE HERE */
  int bit11 = (instruction.btype.imm5 & 0x1) << 11;
  int bit1_al_4 = ((instruction.btype.imm5 >> 1) & 0x0F) << 1;
  int bits5_al_10 = (instruction.btype.imm7 & 0x3F) << 5;
  int bit12 = ((instruction.btype.imm7 >> 6) & 0x1) << 12;
  int imm = bit11 | bit1_al_4 | bits5_al_10 | bit12;
  return bitExtender(imm, 13);
}


int get_jump_distance(Instruction instruction) {
  /* YOUR CODE HERE */
  int bit20 = ((instruction.jtype.imm >> 19) & 0x1) << 20;
  int bit10_al_1 = ((instruction.jtype.imm >> 9) & 0x3FF) << 1;
  int bit11 = ((instruction.jtype.imm >> 8) & 0x1) << 11;
  int bit19_al_12 = (instruction.jtype.imm & 0xFF) << 12;
  int imm = bit20 | bit10_al_1 | bit11 | bit19_al_12;
  return bitExtender(imm, 20);
}

/* Offset in BYTES */
/* Used both in load and store */

int get_memory_offset(Instruction instruction) {
  /* YOUR CODE HERE */
  int bit11_al_5 = (instruction.stype.imm7 & 0x7F) << 5;
  int bit4_al_0 = instruction.stype.imm5;
  int imm = bit11_al_5 | bit4_al_0;

  if (instruction.stype.imm7 & 0x40) {
    imm = imm | 0xFFFFF000;
  }
  return imm;
}


void handle_invalid_instruction(Instruction instruction) {
  printf("Invalid Instruction: 0x%08x\n", instruction.bits);
}


void handle_invalid_read(Address address) {
  printf("Bad Read. Address: 0x%08x\n", address);
  exit(-1);
}


void handle_invalid_write(Address address) {
  printf("Bad Write. Address: 0x%08x\n", address);
  exit(-1);
}