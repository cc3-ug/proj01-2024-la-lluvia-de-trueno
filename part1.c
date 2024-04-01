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
    
    switch(funct3) {
        case F3_ADD:
            if (funct7 == F7_ADD) {
                printf("add\tx%d, x%d, x%d\n", rd, rs1, rs2);
            } else if (funct7 == F7_SUB) {
                printf("sub\tx%d, x%d, x%d\n", rd, rs1, rs2);
            } else {
                handle_invalid_instruction(instruction);
            }
            break;
        case F3_SLL:
            if (funct7 == F7_SLL) {
                printf("sll\tx%d, x%d, x%d\n", rd, rs1, rs2);
            } else {
                handle_invalid_instruction(instruction);
            }
            break;
        // se pueden agregar casos para otras funciones R-Type
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}


void write_itype_except_load(Instruction instruction) {
    unsigned int opcode = get_opcode(instruction);
    unsigned int funct3 = get_funct3(instruction);
    unsigned int rd = get_rd(instruction);
    unsigned int rs1 = get_rs1(instruction);
    int imm = get_imm_I(instruction);
    
    switch(opcode) {
        case OP_IMM:
            switch(funct3) {
                case F3_ADDI:
                    printf("addi\tx%d, x%d, %d\n", rd, rs1, imm);
                    break;
                // Agregar otros casos para instrucciones de tipo I
                default:
                    handle_invalid_instruction(instruction);
                    break;
            }
            break;
        // Agregar otros opcodes de tipo I si es necesario
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}


void write_load(Instruction instruction) {
    unsigned int opcode = get_opcode(instruction);
    unsigned int funct3 = get_funct3(instruction);
    unsigned int rd = get_rd(instruction);
    unsigned int rs1 = get_rs1(instruction);
    int imm = get_imm_I(instruction);
    
    switch(opcode) {
        case OP_LOAD:
            switch(funct3) {
                case F3_LB:
                    printf("lb\tx%d, %d(x%d)\n", rd, imm, rs1);
                    break;
                // Agregar otros casos para instrucciones de carga
                default:
                    handle_invalid_instruction(instruction);
                    break;
            }
            break;
        // Agregar otros opcodes de carga si es necesario
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}


void write_store(Instruction instruction) {
    unsigned int opcode = get_opcode(instruction);
    unsigned int funct3 = get_funct3(instruction);
    unsigned int rs1 = get_rs1(instruction);
    unsigned int rs2 = get_rs2(instruction);
    int imm = get_imm_S(instruction);
    
    switch(opcode) {
        case OP_STORE:
            switch(funct3) {
                case F3_SB:
                    printf("sb\tx%d, %d(x%d)\n", rs2, imm, rs1);
                    break;
                // Agregar otros casos para instrucciones de almacenamiento
                default:
                    handle_invalid_instruction(instruction);
                    break;
            }
            break;
        // Agregar otros opcodes de almacenamiento si es necesario
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}


void write_branch(Instruction instruction) {
    unsigned int opcode = get_opcode(instruction);
    unsigned int funct3 = get_funct3(instruction);
    unsigned int rs1 = get_rs1(instruction);
    unsigned int rs2 = get_rs2(instruction);
    int imm = get_imm_B(instruction);
    
    switch(opcode) {
        case OP_BRANCH:
            switch(funct3) {
                case F3_BEQ:
                    printf("beq\tx%d, x%d, %d\n", rs1, rs2, imm);
                    break;
                // Agregar otros casos para instrucciones de salto condicional
                default:
                    handle_invalid_instruction(instruction);
                    break;
            }
            break;
        // Agregar otros opcodes de salto condicional si es necesario
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}


/* For the writes, probably a good idea to take a look at utils.h */

void write_auipc(Instruction instruction) {
  /* YOUR CODE HERE */
   unsigned int rd = get_rd(instruction);
   int imm = get_imm_U(instruction);
   printf("auipc\tx%d, %d\n", rd, imm);
}


void write_lui(Instruction instruction) {
  /* YOUR CODE HERE */
    unsigned int rd = get_rd(instruction);
    int imm = get_imm_U(instruction);
    printf("lui\tx%d, %d\n", rd, imm);
}


void write_jalr(Instruction instruction) {
  /* YOUR CODE HERE */
    unsigned int rd = get_rd(instruction);
    unsigned int rs1 = get_rs1(instruction);
    int imm = get_imm_I(instruction);
    printf("jalr\tx%d, x%d, %d\n", rd, rs1, imm);
}


void write_jal(Instruction instruction) {
  /* YOUR CODE HERE */
    unsigned int rd = get_rd(instruction);
    int imm = get_imm_J(instruction);
    printf("jal\tx%d, %d\n", rd, imm);
}


void write_ecall(Instruction instruction) {
  /* YOUR CODE HERE */
   printf("ecall\n")
}


void print_rtype(char *name, Instruction instruction) {
  /* YOUR CODE HERE */
    unsigned int rd = get_rd(instruction);
    unsigned int rs1 = get_rs1(instruction);
    unsigned int rs2 = get_rs2(instruction);
    printf("%s\tx%d, x%d, x%d\n", name, rd, rs1, rs2);
}


void print_itype_except_load(char *name, Instruction instruction, int imm) { 
 /* YOUR CODE HERE */
    unsigned int rd = get_rd(instruction);
    unsigned int rs1 = get_rs1(instruction);
    printf("%s\tx%d, x%d, %d\n", name, rd, rs1, imm);
}

void print_load(char *name, Instruction instruction) {
 /* YOUR CODE HERE */
    unsigned int rd = get_rd(instruction);
    int imm = get_imm_I(instruction);
    unsigned int rs1 = get_rs1(instruction);
    printf("%s\tx%d, %d(x%d)\n", name, rd, imm, rs1);
}

void print_store(char *name, Instruction instruction) {
 /* YOUR CODE HERE */
    unsigned int rs1 = get_rs1(instruction);
    unsigned int rs2 = get_rs2(instruction);
    int imm = get_imm_S(instruction);
    printf("%s\tx%d, %d(x%d)\n", name, rs2, imm, rs1);
}

void print_branch(char *name, Instruction instruction) {
 /* YOUR CODE HERE */
    unsigned int rs1 = get_rs1(instruction);
    unsigned int rs2 = get_rs2(instruction);
    int imm = get_imm_B(instruction);
    printf("%s\tx%d, x%d, %d\n", name, rs1, rs2, imm);
}
