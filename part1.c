#include <stdio.h> // for stderr
#include <stdlib.h> // for exit()
#include "types.h"
#include "utils.h"


// forward declarations
void print_rtype(char *, Instruction);
void print_itype_except_load(char *, Instruction, int);
void print_load(char *, Instruction);
void print_store(char *, Instruction);
void print_branch(char *, Instruction);
void write_rtype(Instruction);
void write_itype_except_load(Instruction);
void write_load(Instruction);
void write_store(Instruction);
void write_branch(Instruction);
void write_auipc(Instruction);
void write_lui(Instruction);
void write_jalr(Instruction);
void write_jal(Instruction);
void write_ecall(Instruction);


void decode_instruction(Instruction instruction) {
    switch(instruction.opcode) {
        case 0b0110011: // R-type
            write_rtype(instruction);
            break;
        case 0b0010011: // I-type except load
            write_itype_except_load(instruction);
            break;
        case 0b0000011: // Load
            write_load(instruction);
            break;
        case 0b0100011: // Store
            write_store(instruction);
            break;
        case 0b1100011: // Branch
            write_branch(instruction);
            break;
        case 0b0110111: // AUIPC
            write_auipc(instruction);
            break;
        case 0b0110110: // LUI
            write_lui(instruction);
            break;
        case 0b1100111: // JALR
            write_jalr(instruction);
            break;
        case 0b1101111: // JAL
            write_jal(instruction);
            break;
        case 0b1110011: // ECALL
            write_ecall(instruction);
            break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}


void write_rtype(Instruction instruction) {
    
    switch(instruction.rtype.funct3) {
        case 0x0:
	       switch(instruction.rtype.funct3) {
	          case 0x0:
	            print_rtype("add", instruction);
	            break;
	            case 0x01:
	            print_rtype("mul", instruction);
	            break;
	            case 0x20:
	            print_rtype("sub", instruction);
	            break;
    }
}


void write_itype_except_load(Instruction instruction) {
    
    switch(instruction.itype.funct3) {
        case 0x0:
            print_itype_except_load("addi", instruction, instruction.itype.imm);
            break;
        case 0x1:
            print_itype_except_load("slli", instruction, instruction.itype.imm);
            break;
        // Otros casos para funct3 según sea necesario
    }
}


void write_load(instruction instruction) {
    
    switch(instruction.itype.funct3) {
        case 0x0:
            print_load("lb", instruction);
            break;
        case 0x1:
            print_load("lh", instruction);
            break;
        // Otros casos para funct3 según sea necesario
    }
}


void write_store(Instruction instruction) {
    
    switch(instruction.stype.funct3) {
        case 0x0:
            print_store("sb", instruction);
            break;
        case 0x1:
            print_store("sh", instruction);
            break;
        // Otros casos para funct3 según sea necesario
    }
}


void write_branch(Instruction instruction) {
    
    switch(instruction.btype.funct3) {
        case 0x0:
            print_branch("beq", instruction);
            break;
        case 0x1:
            print_branch("bne", instruction);
            break;
        // Otros casos para funct3 según sea necesario
    }
}


/* For the writes, probably a good idea to take a look at utils.h */

void write_auipc(instruction instruction) {
  /* YOUR CODE HERE */
   printf(AUIPC_FORMAT, instruction.utype.rd, instruction.utype.imm);
}


void write_lui(instruction instruction) {
  /* YOUR CODE HERE */
    printf(LUI_FORMAT, instruction.utype.rd, instruction.utype.imm);
}


void write_jalr(instruction instruction) {
  /* YOUR CODE HERE */
    printf(JALR_FORMAT, instruction.itype.rd, instruction.itype.rs1, bitExtender(instruction.itype.imm, 12));
}


void write_jal(instruction instruction) {
  /* YOUR CODE HERE */
    printf(JAL_FORMAT, instruction.jtype.rd, get_jump_distance(instruction));
}


void write_ecall(instruction instruction) {
  /* YOUR CODE HERE */
   printf(ECALL_FORMAT);
}


void print_rtype(char *name, instruction instruction) {
  /* YOUR CODE HERE */
    printf(RTYPE_FORMAT, name, instruction.rtype.rd, instruction.rtype.rs1, instruction.rtype.rs2);
}


void print_itype_except_load(char *name, instruction instruction, int imm) { 
 /* YOUR CODE HERE */
    printf(ITYPE_FORMAT, name, instruction.itype.rd, instruction.itype.rs1, bitExtender(instructon.itype.imm, 12));
}

void print_load(char *name, instruction instruction) {
 /* YOUR CODE HERE */
    printf(JAL_FORMAT, name, instruction.itype.rd, bitExtender(instructon.itype.imm, 12), instruction.itype.rs1);
}

void print_store(char *name, instruction instruction) {
 /* YOUR CODE HERE */
    printf(MEM_FORMAT, name, instruction.stype.rs2, get_jump_distance(instruction), instruction.stype.rs1);
}

void print_branch(char *name, instruction instruction) {
 /* YOUR CODE HERE */
    printf(BRANCH_FORMAT, name, instruction.btype.rs1, instruction.btype.rs2, get_jump_distance(instruction));
}
