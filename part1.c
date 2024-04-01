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
    unsigned int opcode = get_opcode(instruction);
    switch(opcode) {
        case OP_RTYPE:
            write_rtype(instruction);
            break;
        case OP_ITYPE:
            write_itype_except_load(instruction);
            break;
        case OP_LOAD:
            write_load(instruction);
            break;
        case OP_STORE:
            write_store(instruction);
            break;
        case OP_BRANCH:
            write_branch(instruction);
            break;
        case OP_AUIPC:
            write_auipc(instruction);
            break;
        case OP_LUI:
            write_lui(instruction);
            break;
        case OP_JALR:
            write_jalr(instruction);
            break;
        case OP_JAL:
            write_jal(instruction);
            break;
        case OP_ECALL:
            write_ecall(instruction);
            break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}


void write_rtype(Instruction instruction) {
    unsigned int funct3 = get_funct3(instruction);
    unsigned int funct7 = get_funct7(instruction);
    unsigned int rs1 = get_rs1(instruction);
    unsigned int rs2 = get_rs2(instruction);
    unsigned int rd = get_rd(instruction);
    
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
    unsigned int opcode = get_opcode(instruction);
    unsigned int funct3 = get_funct3(instruction);
    unsigned int rd = get_rd(instruction);
    unsigned int rs1 = get_rs1(instruction);
    int imm = get_imm_I(instruction);
    
    switch(instruction.itype.funct3) {
        case 0x0:
            print_itype("addi", instruction);
            break;
        case 0x1:
            print_itype("slli", instruction);
            break;
        // Otros casos para funct3 según sea necesario
    }
}


void write_load(Instruction instruction) {
    unsigned int opcode = get_opcode(instruction);
    unsigned int funct3 = get_funct3(instruction);
    unsigned int rd = get_rd(instruction);
    unsigned int rs1 = get_rs1(instruction);
    int imm = get_imm_I(instruction);
    
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
    unsigned int opcode = get_opcode(instruction);
    unsigned int funct3 = get_funct3(instruction);
    unsigned int rs1 = get_rs1(instruction);
    unsigned int rs2 = get_rs2(instruction);
    int imm = get_imm_S(instruction);
    
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
    unsigned int opcode = get_opcode(instruction);
    unsigned int funct3 = get_funct3(instruction);
    unsigned int rs1 = get_rs1(instruction);
    unsigned int rs2 = get_rs2(instruction);
    int imm = get_imm_B(instruction);
    
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

void write_auipc(Instruction instruction) {
  /* YOUR CODE HERE */
   printf(AUIPC_FORMAT, instruction.utype.rd. instruction.utype.imm);
}


void write_lui(Instruction instruction) {
  /* YOUR CODE HERE */
    printf(LUI_FORMAT, instruction.utype.rd, instruction.utype.imm);
}


void write_jalr(Instruction instruction) {
  /* YOUR CODE HERE */
    printf(JALR_FORMAT, instruction.itype.rd, instruction.itype.rs1, instruction.itype.imm);
}


void write_jal(Instruction instruction) {
  /* YOUR CODE HERE */
    printf(JAL_FORMAT, instruction.ujtype.rd, instruction.ujtype.imm);
}


void write_ecall(Instruction instruction) {
  /* YOUR CODE HERE */
   printf(ECALL_FORMAT);
}


void print_rtype(char *name, Instruction instruction) {
  /* YOUR CODE HERE */
    printf(RTYPE_FORMAT, name, rd, rs1, rs2);
}


void print_itype_except_load(char *name, Instruction instruction, int imm) { 
 /* YOUR CODE HERE */
    printf(ITYPE_EXCEPT_LOAD_FORMAT, name, rd, rs1, imm);
}

void print_load(char *name, Instruction instruction) {
 /* YOUR CODE HERE */
    printf(LOAD_FORMAT, name, rd, imm, rs1);
}

void print_store(char *name, Instruction instruction) {
 /* YOUR CODE HERE */
    printf(STORE_FORMAT, name, rs2, imm, rs1);
}

void print_branch(char *name, Instruction instruction) {
 /* YOUR CODE HERE */
    printf(BRANCH_FORMAT, name, rs1, rs2, imm);
}
