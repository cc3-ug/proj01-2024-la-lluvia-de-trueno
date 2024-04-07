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
        case 0b0110110:  // LUI
            execute_lui(instruction, processor);
            break;
        case 0b0110111: // AUIPC
            execute_auipc(instruction, processor);
            break;
        case 0b1101111: // JAL
            execute_jal(instruction, processor);
            break;
        case 0b1100111: // JALR
            execute_jalr(instruction, processor);
            break;
        case 0b1100011: // Branch
            execute_branch(instruction, processor);
            break;
        case 0b0000011: // Load
            execute_load(instruction, processor, memory);
            break;
        case 0b0100011: // Store
            execute_store(instruction, processor, memory);
            break;
        case 0b0010011: // I-type except load
            execute_itype_except_load(instruction, processor);
            break;
        case 0b1110011: // ECALL
            execute_ecall(processor, memory);
            break;
        case 0b0110011: // R-type
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
        case 0x0: // R-type ADD, SUB
            switch (instruction.rtype.funct7) {
                case 0x0:
                    processor->R[rd] = (sWord)processor->R[rs1] + (sWord)processor->R[rs2];
                    break;
                case 0x20:
                    processor->R[rd] = (sWord)processor->R[rs1] - (sWord)processor->R[rs2];
                    break;
                case 0x01:
	            processor->R[rd] = (sWord)processor->R[rs1] * (sWord)processor->R[rs2];
	            break;
                default:
                    handle_invalid_instruction(instruction);
                    break;
            }
            break;
        case 0x1:
	       switch(instruction.rtype.funct7) {
	       case 0x0: //sll
	            processor->R[rd] = (sWord)processor->R[rs1] << (sWord)processor->R[rs2];
	            break;
	            case 0x1: //mulh
	            processor->R[rd] = (sWord)processor->R[rs1] * (sWord)processor->R[rs2];
	            break;
	   } 
	 case 0x2:
	       switch(instruction.rtype.funct7) {
	            case 0x0: //slt
	            processor->R[rd] = (sWord)processor->R[rs1] < (sWord)processor->R[rs2];
	            break;
	   }
	case 0x4:
	       switch(instruction.rtype.funct7) {
	            case 0x0: //xor
	            processor->R[rd] = (sWord)processor->R[rs1] ^ (sWord)processor->R[rs2];
	            break;
	            case 0x4: //div
	            processor->R[rd] = (sWord)processor->R[rs1] / (sWord)processor->R[rs2];
	            break;
	    }        
	case 0x5:
	       switch(instruction.rtype.funct7) {
	            case 0x0: //srl
	            processor->R[rd] = (sWord)processor->R[rs1] >> (sWord)processor->R[rs2];
	            break;
	            case 0x5: //sra
	            processor->R[rd] = (sWord)processor->R[rs1] >> (sWord)processor->R[rs2];
	            break;
	   }     
	case 0x6:
	       switch(instruction.rtype.funct7) {      
	            case 0x0: //or
	            processor->R[rd] = (sWord)processor->R[rs1] | (sWord)processor->R[rs2];
	            break;
	            case 0x6: //rem
	            processor->R[rd] = (sWord)processor->R[rs1] % (sWord)processor->R[rs2];
	            break;
	   }         
	case 0x7:
	       switch(instruction.rtype.funct7) {       
	            case 0x7: //and
	            processor->R[rd] = (sWord)processor->R[rs1] & (sWord)processor->R[rs2];
	            break;
	    }
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
    processor->PC += 4;
}


void execute_itype_except_load(Instruction instruction, Processor *processor) {
    switch(instruction.itype.funct3) {
        case 0x0: //addi
            processor->R[instruction.itype.rd] = (sWord)processor->R[instruction.itype.rs1] + instruction.itype.imm;
            break;
        case 0x1: //slli
            processor->R[instruction.itype.rd] = (sWord)processor->R[instruction.itype.rs1] << instruction.itype.imm;
            break;
        case 0x2: //slti
            processor->R[instruction.itype.rd] = (((sWord)processor->R[instruction.itype.rs1] & instruction.itype.imm) == Instruction.itype.imm) ? 1 : 0
            break;
        case 0x4: //xori
            processor->R[instruction.itype.rd] = (sWord)processor->R[instruction.itype.rs1] ^ instruction.itype.imm;
            break;          
        case 0x6: //ori
            processor->R[instruction.itype.rd] = (sWord)processor->R[instruction.itype.rs1] | instruction.itype.imm;
            break;  
        case 0x7: //andi
            processor->R[instruction.itype.rd] = (sWord)processor->R[instruction.itype.rs1] & instruction.itype.imm;
            break;  
        case 0x5: //srai - srli
	     if (instruction.itype.imm >> 5 == 0b0100000){ 
	     processor->R[instruction.itype.rd] = (sWord)processor->R[instruction.itype.rs1] >> instruction.itype.imm; 
	     break;      
	     }
	      processor->R[instruction.itype.rd] = (sWord)processor->R[instruction.itype.rs1] >> instruction.itype.imm;
    }
    processor->PC += 4;
}

void execute_ecall(Processor *processor, Byte *memory) {
    switch (processor->R[REG_A7]) { // A7 holds the ecall number
        case 1: // Print integer
            printf("%d", (int)processor->R[REG_A0]);
            break;
        case 10: // Exit
            exit(0);
            break;
        default:
            printf("Illegal ecall number %d\n", processor->R[REG_A7]);
            exit(-1);
            break;
    }
}


void execute_branch(Instruction instruction, Processor *processor) {
    Immediate imm = sign_extend_number(concat_bits(instruction.branch.imm12, instruction.branch.imm11, instruction.branch.imm10_5, instruction.branch.imm4_1, 0), 13);
    switch (instruction.branch.funct3) {
        case 0b1100011: // Branch BEQ
            if (processor->R[instruction.branch.rs1] == processor->R[instruction.branch.rs2])
                processor->PC += imm;
            else
                processor->PC += 4;
            break;
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}


void execute_load(Instruction instruction, Processor *processor, Byte *memory) {
    Immediate imm = sign_extend_number(instruction.load.imm, 12);
    switch (instruction.load.funct3) {
        case 0b0000011: // Load LB
            processor->R[instruction.load.rd] = sign_extend_number(memory[processor->R[instruction.load.rs1] + imm], 8);
            break;
        case 0b0000011: // Load LH
            processor->R[instruction.load.rd] = sign_extend_number(concat_bytes(memory[processor->R[instruction.load.rs1] + imm + 1], memory[processor->R[instruction.load.rs1] + imm]), 16);
            break;
        case LW:
            processor->R[instruction.load.rd] = concat_bytes(memory[processor->R[instruction.load.rs1] + imm + 3], memory[processor->R[instruction.load.rs1] + imm + 2], memory[processor->R[instruction.load.rs1] + imm + 1], memory[processor->R[instruction.load.rs1] + imm]);
            break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }
    processor->PC += 4;
}


void execute_store(Instruction instruction, Processor *processor, Byte *memory) {
    Immediate imm = sign_extend_number(concat_bits(instruction.store.imm12, instruction.store.imm11, instruction.store.imm10_5, instruction.store.imm4_1, 0), 13);
    switch (instruction.store.funct3) {
        case 0b0100011: // Store SB
            memory[processor->R[instruction.store.rs1] + imm] = (Byte)processor->R[instruction.store.rs2];
            break;
        case 0b0100011: // Store SH
            memory[processor->R[instruction.store.rs1] + imm] = (Byte)processor->R[instruction.store.rs2];
            memory[processor->R[instruction.store.rs1] + imm + 1] = (Byte)(processor->R[instruction.store.rs2] >> 8);
            break;
        case 0b0100011: // Store SW
            memory[processor->R[instruction.store.rs1] + imm] = (Byte)processor->R[instruction.store.rs2];
            memory[processor->R[instruction.store.rs1] + imm + 1] = (Byte)(processor->R[instruction.store.rs2] >> 8);
            memory[processor->R[instruction.store.rs1] + imm + 2] = (Byte)(processor->R[instruction.store.rs2] >> 16);
            memory[processor->R[instruction.store.rs1] + imm + 3] = (Byte)(processor->R[instruction.store.rs2] >> 24);
            break;
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
    processor->PC += 4;
}


void execute_jalr(Instruction instruction, Processor *processor) {
    Immediate imm = sign_extend_number(instruction.itype_except_load.imm, 12);
    processor->R[instruction.itype_except_load.rd] = processor->PC + 4;
    processor->PC = (processor->R[instruction.itype_except_load.rs1] + imm) & (~1); // PC obtiene la dirección de la instrucción que sigue a la que se está ejecutando
}

void execute_jal(Instruction instruction, Processor *processor) {
    Immediate imm = sign_extend_number(concat_bits(instruction.ujump.imm20, instruction.ujump.imm19_12, instruction.ujump.imm11, instruction.ujump.imm10_1, 0), 21);
    processor->R[instruction.ujump.rd] = processor->PC + 4;
    processor->PC += imm;
}

void execute_auipc(Instruction instruction, Processor *processor) {
    Immediate imm = sign_extend_number(instruction.utype.imm, 20);
    processor->R[instruction.utype.rd] = processor->PC + imm;
    processor->PC += 4;
}

void execute_lui(Instruction instruction, Processor *processor) {
    Immediate imm = sign_extend_number(instruction.utype.imm, 20);
    processor->R[instruction.utype.rd] = imm;
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
