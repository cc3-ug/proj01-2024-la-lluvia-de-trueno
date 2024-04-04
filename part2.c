#include <stdio.h> // for stderr
#include <stdlib.h> // for exit()
#include "types.h"
#include "utils.h"
#include "riscv.h"


// forward declarations
void execute_rtype(Instruction, Processor *);
void execute_itype_except_load(Instruction, Processor *);
void execute_branch(Instruction, Processor *);
void execute_jalr(Instruction, Processor *);
void execute_jal(Instruction, Processor *);
void execute_load(Instruction, Processor *, Byte *);
void execute_store(Instruction, Processor *, Byte *);
void execute_ecall(Processor *, Byte *);
void execute_auipc(Instruction, Processor *);
void execute_lui(Instruction, Processor *);


void execute_instruction(Instruction instruction, Processor *processor, Byte *memory) {
    switch (instruction.opcode) {
        case LUI:
            execute_lui(instruction, processor);
            break;
        case AUIPC:
            execute_auipc(instruction, processor);
            break;
        case JAL:
            execute_jal(instruction, processor);
            break;
        case JALR:
            execute_jalr(instruction, processor);
            break;
        case BRANCH:
            execute_branch(instruction, processor);
            break;
        case LOAD:
            execute_load(instruction, processor, memory);
            break;
        case STORE:
            execute_store(instruction, processor, memory);
            break;
        case OP_IMM:
            execute_itype_except_load(instruction, processor);
            break;
        case ECALL:
            execute_ecall(processor, memory);
            break;
        case OP:
            execute_rtype(instruction, processor);
            break;
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}

void execute_rtype(Instruction instruction, Processor *processor) {
    switch (instruction.rtype.funct3) {
        case ADD_SUB:
            processor->reg_file[instruction.rtype.rd] = (instruction.rtype.funct7 == 0) ?
                                                          processor->reg_file[instruction.rtype.rs1] + processor->reg_file[instruction.rtype.rs2] :
                                                          processor->reg_file[instruction.rtype.rs1] - processor->reg_file[instruction.rtype.rs2];
            break;
        case SLL:
            processor->reg_file[instruction.rtype.rd] = processor->reg_file[instruction.rtype.rs1] << (processor->reg_file[instruction.rtype.rs2] & 0x1F);
            break;
        // para agg más casos para otras instrucciones tipo R aquí
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
    processor->PC += 4;
}


void execute_itype_except_load(Instruction instruction, Processor *processor) {
    switch (instruction.itype_except_load.funct3) {
        case ADDI:
            processor->reg_file[instruction.itype_except_load.rd] = processor->reg_file[instruction.itype_except_load.rs1] + sign_extend_number(instruction.itype_except_load.imm, 12);
            break;
        case SLTI:
            processor->reg_file[instruction.itype_except_load.rd] = (processor->reg_file[instruction.itype_except_load.rs1] < sign_extend_number(instruction.itype_except_load.imm, 12)) ? 1 : 0;
            break;
        // Agregue más casos para otras instrucciones de tipo I-type
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
    processor->PC += 4;
}


void execute_ecall(Processor *processor, Byte *memory) {
    switch (processor->reg_file[REG_A7]) { // A7 holds the ecall number
        case 1: // Print integer
            printf("%d", (int)processor->reg_file[REG_A0]);
            break;
        case 10: // Exit
            exit(0);
            break;
        default:
            printf("Illegal ecall number %d\n", processor->reg_file[REG_A7]);
            exit(-1);
            break;
    }
}


void execute_branch(Instruction instruction, Processor *processor) {
    Immediate imm = sign_extend_number(concat_bits(instruction.branch.imm12, instruction.branch.imm11, instruction.branch.imm10_5, instruction.branch.imm4_1, 0), 13);
    switch (instruction.branch.funct3) {
        case BEQ:
            if (processor->reg_file[instruction.branch.rs1] == processor->reg_file[instruction.branch.rs2])
                processor->PC += imm;
            else
                processor->PC += 4;
            break;
        // Agregue más casos para otras instrucciones de branch
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}


void execute_load(Instruction instruction, Processor *processor, Byte *memory) {
    Immediate imm = sign_extend_number(instruction.load.imm, 12);
    switch (instruction.load.funct3) {
        case LB:
            processor->reg_file[instruction.load.rd] = sign_extend_number(memory[processor->reg_file[instruction.load.rs1] + imm], 8);
            break;
        case LH:
            processor->reg_file[instruction.load.rd] = sign_extend_number(concat_bytes(memory[processor->reg_file[instruction.load.rs1] + imm + 1], memory[processor->reg_file[instruction.load.rs1] + imm]), 16);
            break;
        case LW:
            processor->reg_file[instruction.load.rd] = concat_bytes(memory[processor->reg_file[instruction.load.rs1] + imm + 3], memory[processor->reg_file[instruction.load.rs1] + imm + 2], memory[processor->reg_file[instruction.load.rs1] + imm + 1], memory[processor->reg_file[instruction.load.rs1] + imm]);
            break;
        // Agregue más casos para otras instrucciones de load 
        default:
            handle_invalid_instruction(instruction);
            break;
    }
    processor->PC += 4;
}


void execute_store(Instruction instruction, Processor *processor, Byte *memory) {
    Immediate imm = sign_extend_number(concat_bits(instruction.store.imm12, instruction.store.imm11, instruction.store.imm10_5, instruction.store.imm4_1, 0), 13);
    switch (instruction.store.funct3) {
        case SB:
            memory[processor->reg_file[instruction.store.rs1] + imm] = (Byte)processor->reg_file[instruction.store.rs2];
            break;
        case SH:
            memory[processor->reg_file[instruction.store.rs1] + imm] = (Byte)processor->reg_file[instruction.store.rs2];
            memory[processor->reg_file[instruction.store.rs1] + imm + 1] = (Byte)(processor->reg_file[instruction.store.rs2] >> 8);
            break;
        case SW:
            memory[processor->reg_file[instruction.store.rs1] + imm] = (Byte)processor->reg_file[instruction.store.rs2];
            memory[processor->reg_file[instruction.store.rs1] + imm + 1] = (Byte)(processor->reg_file[instruction.store.rs2] >> 8);
            memory[processor->reg_file[instruction.store.rs1] + imm + 2] = (Byte)(processor->reg_file[instruction.store.rs2] >> 16);
            memory[processor->reg_file[instruction.store.rs1] + imm + 3] = (Byte)(processor->reg_file[instruction.store.rs2] >> 24);
            break;
        // Agregue más casos para obtener otras instrucciones de store 
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
    processor->PC += 4;
}


void execute_jalr(Instruction instruction, Processor *processor) {
    Immediate imm = sign_extend_number(instruction.itype_except_load.imm, 12);
    processor->reg_file[instruction.itype_except_load.rd] = processor->PC + 4;
    processor->PC = (processor->reg_file[instruction.itype_except_load.rs1] + imm) & (~1); // PC obtiene la dirección de la instrucción que sigue a la que se está ejecutando
}

void execute_jal(Instruction instruction, Processor *processor) {
    Immediate imm = sign_extend_number(concat_bits(instruction.ujump.imm20, instruction.ujump.imm19_12, instruction.ujump.imm11, instruction.ujump.imm10_1, 0), 21);
    processor->reg_file[instruction.ujump.rd] = processor->PC + 4;
    processor->PC += imm;
}

void execute_auipc(Instruction instruction, Processor *processor) {
    Immediate imm = sign_extend_number(instruction.utype.imm, 20);
    processor->reg_file[instruction.utype.rd] = processor->PC + imm;
    processor->PC += 4;
}

void execute_lui(Instruction instruction, Processor *processor) {
    Immediate imm = sign_extend_number(instruction.utype.imm, 20);
    processor->reg_file[instruction.utype.rd] = imm;
    processor->PC += 4;
}


/* Checks that the address is aligned correctly */
int check(Address address, Alignment alignment) {
  if (address > 0 && address < MEMORY_SPACE) {
    // byte align
    if (alignment == LENGTH_BYTE) return 1;
    // half align
    if (alignment == LENGTH_HALF_WORD) return ((address % 2) == 0);
    // word align
    if (alignment == LENGTH_WORD) return ((address % 4) == 0);
  }
  return 0;
}


void store(Byte *memory, Address address, Alignment alignment, Word value, int check_align) {
    if ((check_align && !check(address, alignment)) || (address >= MEMORY_SPACE)) {
        handle_invalid_write(address);
        exit(-1);
    }

    switch (alignment) {
        case LENGTH_BYTE:
            memory[address] = (Byte)value;
            break;
        case LENGTH_HALF_WORD:
            memory[address] = (Byte)value;
            memory[address + 1] = (Byte)(value >> 8);
            break;
        case LENGTH_WORD:
            memory[address] = (Byte)value;
            memory[address + 1] = (Byte)(value >> 8);
            memory[address + 2] = (Byte)(value >> 16);
            memory[address + 3] = (Byte)(value >> 24);
            break;
        default:
            handle_invalid_write(address);
            exit(-1);
            break;
    }
}


Word load(Byte *memory, Address address, Alignment alignment, int check_align) {
    if ((check_align && !check(address, alignment)) || (address >= MEMORY_SPACE)) {
        handle_invalid_read(address);
        exit(-1);
    }

    Word value = 0;
    switch (alignment) {
        case LENGTH_BYTE:
            value = memory[address];
            break;
        case LENGTH_HALF_WORD:
            value = concat_bytes(memory[address + 1], memory[address]);
            break;
        case LENGTH_WORD:
            value = concat_bytes(memory[address + 3], memory[address + 2], memory[address + 1], memory[address]);
            break;
        default:
            handle_invalid_read(address);
            exit(-1);
            break;
    }
    return value;
}
