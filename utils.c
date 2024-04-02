#include "utils.h"
#include <stdio.h>
#include <stdlib.h>


//sign extends a bitfield with given size
/* You may find implementing this function helpful */
int bitExtender(unsigned int field, unsigned int size) {
    if (size >= 32) { //si es mayor o igual no se necesita extender 
        return field;
    }
    int sign = (field >> (31 - size)) & 1; // se mueve hacia la derecha
    int extend = (field << size) | (sign << (32 - size)); // lo mueve hacia la izquierda
    return extend;//da el dato extendido
}

/* Distances in BYTES */

int get_branch_distance(Instruction instruction) {
int imm = (instruction.bits >> 6) & 0x1f;
    int offset = (instruction.bits >> 11) & 0x1fff;
    if (instruction.bits & (1 << 10)) {
        return -(bitExtender(offset, 13) + (imm << 13));
    }
    return bitExtender(offset, 13) + (imm << 13);
}


int get_jump_distance(Instruction instruction) {
int imm = (instruction.bits >> 12) & 0xfff;
    if (instruction.bits & (1 << 11)) {
        return -(bitExtender(imm, 12) << 2);
    }
    return bitExtender(imm, 12) << 2;
}

/* Offset in BYTES */
/* Used both in load and store */

int get_memory_offset(Instruction instruction) {
int imm = (instruction.bits >> 3) & 0x1ff;
    if (instruction.bits & (1 << 2)) {
        return -(bitExtender(imm, 10) << 2);
    }
    return bitExtender(imm, 10) << 2;
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
