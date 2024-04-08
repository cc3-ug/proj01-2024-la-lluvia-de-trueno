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
        case 0b0110111:  // LUI
            processor->PC += 4;
            execute_lui(instruction, processor);
            break;
        case 0b0010111: // AUIPC
            execute_auipc(instruction, processor);
            processor->PC += 4;
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
            processor->PC += 4;
            break;
        case 0b0010011: // I-type except load
            execute_itype_except_load(instruction, processor);
            processor->PC += 4;
            break;
        case 0b1110011: // ECALL
            execute_ecall(processor, memory);
            break;
        case 0b0110011: // R-type
            processor->PC += 4;
            execute_rtype(instruction, processor);
            break;
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}

void execute_rtype(Instruction instruction, Processor *processor) {
    sDouble mul = 0;
    sDouble mulh = 0;
    switch (instruction.rtype.funct3) {
        case 0x0: // R-type ADD, SUB
            switch (instruction.rtype.funct7) {
                case 0x0:
                    processor->R[instruction.rtype.rd] = ((sWord)processor->R[instruction.rtype.rs1]) + ((sWord)processor->R[instruction.rtype.rs2]);
                    break;
                case 0x20:
                    processor->R[instruction.rtype.rd] = ((sWord)processor->R[instruction.rtype.rs1]) - ((sWord)processor->R[instruction.rtype.rs2]);
                    break;
                case 0x01:
	                mul = ((sDouble)processor->R[instruction.rtype.rs1] * (sDouble)processor->R[instruction.rtype.rs2]);
                    processor->R[instruction.rtype.rd] = (int)((mul << 31)>>31); 
	                break;
                default:
                    handle_invalid_instruction(instruction);
                    break;
            }
            break;

        case 0x1:
	       switch(instruction.rtype.funct7) {
	            case 0x0: //sll
	                processor->R[instruction.rtype.rd] = ((sWord)processor->R[instruction.rtype.rs1]) << ((sWord)processor->R[instruction.rtype.rs2]);
	                break;
	            case 0x1: //mulh
	                mulh = ((sDouble)processor->R[instruction.rtype.rs1] * (sDouble)processor->R[instruction.rtype.rs2])>>32;
                    processor->R[instruction.rtype.rd] = mulh; 
	                break;
	        }
            break;

	    case 0x2:
	       switch(instruction.rtype.funct7) {
	            case 0x0: //slt
	            processor->R[instruction.rtype.rd] = (sWord)processor->R[instruction.rtype.rs1] < (sWord)processor->R[instruction.rtype.rs2] ? 1 : 0;
	            break;
	        }
            break;

	 case 0x4:
	       switch(instruction.rtype.funct7) {
	            case 0x0: //xor
	                processor->R[instruction.rtype.rd] = (sWord)processor->R[instruction.rtype.rs1] ^ (sWord)processor->R[instruction.rtype.rs2];
	                break;
	            case 0x1: //div
                    if((sWord)(processor->R[instruction.rtype.rs2] == 0)){
                        processor->R[instruction.rtype.rd] = -1;
                    }
                    else if ((sWord)processor->R[instruction.rtype.rs1] == 0x80000000 && (sWord)processor->R[instruction.rtype.rs2] == 0xffffffff){
                        processor->R[instruction.rtype.rd] = (sWord)processor->R[instruction.rtype.rs1];
                    }
                    else{
                        processor->R[instruction.rtype.rd] = ((sWord)processor->R[instruction.rtype.rs1]) / ((sWord)processor->R[instruction.rtype.rs2]);
                    } 
                break;
	        }
            break;

	case 0x5:
	       switch(instruction.rtype.funct7) {
	            case 0x0: //srl
	                processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs1] >> processor->R[instruction.rtype.rs2];
	                break;
	            case 0x20: //sra
	                processor->R[instruction.rtype.rd] = (sWord)processor->R[instruction.rtype.rs1] >> (sWord)processor->R[instruction.rtype.rs2];
	                break;
	        }
            break;

	case 0x6:
	       switch(instruction.rtype.funct7) {      
	            case 0x0: //or
	                processor->R[instruction.rtype.rd] = (sWord)processor->R[instruction.rtype.rs1] | (sWord)processor->R[instruction.rtype.rs2];
	                break;
	            case 0x1: //rem
                    if((sWord)(processor->R[instruction.rtype.rs2] == 0)){
                        processor->R[instruction.rtype.rd] = (sWord)processor->R[instruction.rtype.rs1];
                    }
                    else if ((sWord)processor->R[instruction.rtype.rs1] == 0x80000000 && (sWord)processor->R[instruction.rtype.rs2] == 0xffffffff){
                        processor->R[instruction.rtype.rd] = 0;
                    }
                    else{
                        processor->R[instruction.rtype.rd] = (sWord)processor->R[instruction.rtype.rs1] % (sWord)processor->R[instruction.rtype.rs2];
                    } 
                    break;
	        }     
            break;

	case 0x7:
	       switch(instruction.rtype.funct7) {       
	            case 0x0: //and
	            processor->R[instruction.rtype.rd] = (sWord)processor->R[instruction.rtype.rs1] & (sWord)processor->R[instruction.rtype.rs2];
	            break;
	        }
            break;

        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }

}


void execute_itype_except_load(Instruction instruction, Processor *processor) {
    switch(instruction.itype.funct3) {
        case 0x0: //addi
            processor->R[instruction.itype.rd] = (sWord)processor->R[instruction.itype.rs1] + bitExtender(instruction.itype.imm, 12);
            break;
        case 0x1: //slli
            processor->R[instruction.itype.rd] = (sWord)processor->R[instruction.itype.rs1] << bitExtender(instruction.itype.imm, 12);
            break;
        case 0x2: //slti
            processor->R[instruction.itype.rd] = (((sWord)processor->R[instruction.itype.rs1] < bitExtender(instruction.itype.imm, 12)) ? 1 : 0);
            break;
        case 0x4: //xori
            processor->R[instruction.itype.rd] = (sWord)processor->R[instruction.itype.rs1] ^ bitExtender(instruction.itype.imm, 12);
            break;          
        case 0x6: //ori
            processor->R[instruction.itype.rd] = (sWord)processor->R[instruction.itype.rs1] | bitExtender(instruction.itype.imm, 12);
            break;  
        case 0x7: //andi
            processor->R[instruction.itype.rd] = (sWord)processor->R[instruction.itype.rs1] & bitExtender(instruction.itype.imm, 12);
            break;  
        case 0x5: //srai - srli
            if (instruction.itype.imm >> 5 == 0b0100000){ 
                processor->R[instruction.itype.rd] = (sWord)processor->R[instruction.itype.rs1] >> bitExtender(instruction.itype.imm, 12); 
                break;      
            }
            processor->R[instruction.itype.rd] = processor->R[instruction.itype.rs1] >> bitExtender(instruction.itype.imm, 12);
            break;
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }

}

void execute_ecall(Processor *processor, Byte *memory) {
    switch (processor->R[10]) { 
        case 1: 
            printf("%d", (int)processor->R[11]);
            processor->PC += 4;
            break;
        case 10: 
            printf("exiting the simulator\n");
            exit(0);
            break;
        default:
            printf("Illegal ecall number %d\n", processor->R[10]);
            exit(-1);
            break;
    }
}


void execute_branch(Instruction instruction, Processor *processor) {
    switch(instruction.btype.funct3) {
        case 0x0: //beq
            if((sWord)processor->R[instruction.rtype.rs1] == (sWord)processor->R[instruction.rtype.rs2]) {
                processor->PC += get_branch_distance(instruction);
            } else {
                processor->PC += 4;
            }
            break;
        case 0x1: //bne
            if((sWord)processor->R[instruction.rtype.rs1] != (sWord)processor->R[instruction.rtype.rs2]) {
                processor->PC += get_branch_distance(instruction);
            } else {
                processor->PC += 4;
            }
            break;
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}


void execute_load(Instruction instruction, Processor *processor, Byte *memory) {
    switch(instruction.itype.funct3) {
        case 0x0: //lb
            processor->R[instruction.itype.rd] = bitExtender(load(memory, (sWord)processor->R[instruction.itype.rs1] + ((sWord)bitExtender(instruction.itype.imm , 12)) , LENGTH_BYTE, 0), 8 );
            processor->PC += 4;
            break;
        case 0x1: //lh
            processor->R[instruction.itype.rd] = bitExtender(load(memory, (sWord)processor->R[instruction.itype.rs1] + ((sWord)bitExtender(instruction.itype.imm , 12)) , LENGTH_HALF_WORD, 0), 16 );
            processor->PC += 4;
            break;
        case 0x2: //lw
            processor->R[instruction.itype.rd] = load(memory, (sWord)processor->R[instruction.itype.rs1] + ((sWord)bitExtender(instruction.itype.imm , 12)) , LENGTH_WORD, 0);
            processor->PC += 4;
            break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}


void execute_store(Instruction instruction, Processor *processor, Byte *memory) {
    switch(instruction.itype.funct3) {
        case 0x0: //sb
            store(memory, ((sWord)processor->R[instruction.stype.rs1]) + ((sWord)get_memory_offset(instruction)), LENGTH_BYTE , processor->R[instruction.stype.rs2], 0);
            break;
        case 0x1: //sh
            store(memory, ((sWord)processor->R[instruction.stype.rs1]) + ((sWord)get_memory_offset(instruction)), LENGTH_HALF_WORD , processor->R[instruction.stype.rs2], 0);
            break;
        case 0x2: //sw
            store(memory, ((sWord)processor->R[instruction.stype.rs1]) + ((sWord)get_memory_offset(instruction)), LENGTH_WORD , processor->R[instruction.stype.rs2], 0);
            break;
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}


void execute_jalr(Instruction instruction, Processor *processor) {
	processor->R[instruction.itype.rd] = processor->PC + 4;  
	processor->PC = (processor->R[instruction.stype.rs1] + bitExtender(instruction.itype.imm, 12));
}

void execute_jal(Instruction instruction, Processor *processor) {
	int jump_address = processor->PC + (sWord)get_jump_distance(instruction);
	processor->R[instruction.jtype.rd] = processor->PC + 4;
	processor->PC = jump_address;
}

void execute_auipc(Instruction instruction, Processor *processor) {
	processor->R[instruction.utype.rd] = (processor->PC + (instruction.utype.imm << 12));
}

void execute_lui(Instruction instruction, Processor *processor) {
	processor->R[instruction.utype.rd] = (instruction.utype.imm << 12);
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
    }

    switch (alignment) {
        case LENGTH_BYTE:
            memory[address] = value & 0xFF;
            break;
        case LENGTH_HALF_WORD:
            memory[address] = value & 0xFF;
            memory[address + 1] = (value >> 8) & 0xFF;
            break;
        case LENGTH_WORD:
            memory[address] = value & 0xFF;
            memory[address + 1] = (value >> 8) & 0xFF;
            memory[address + 2] = (value >> 16) & 0xFF;
            memory[address + 3] = (value >> 24) & 0xFF;
            break;
        default:
            handle_invalid_write(address);
            break;
    }
}


Word load(Byte *memory, Address address, Alignment alignment, int check_align) {
    if ((check_align && !check(address, alignment)) || (address >= MEMORY_SPACE)) {
        handle_invalid_read(address);
    }

    Word value = 0;
    switch (alignment) {
        case LENGTH_BYTE:
            value = value | memory[address];
		    return value;
            break;
        case LENGTH_HALF_WORD:
            value = memory[address];    
	        value = value | (memory[address+1] << 8);
	        return value;
            break;
        case LENGTH_WORD:
            value = memory[address];    
	        value = value | (memory[address+1] << 8);
	        value = value | (memory[address+2] << 16);
	        value = value | (memory[address+3] << 24);
	        return value;
            break;
    }
    return value;
}
