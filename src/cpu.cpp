#include "cpu.h"
#include "bus.h"
#include <iostream>

CPU::CPU(std::shared_ptr<Bus> bus) : bus_(bus), pc_(0x80000000),
                                     pipelining_enabled_(false),
                                     stall_next_cycle_(false),
                                     pipeline_flush_(false) {
    // Initialize registers to zero
    for (auto& reg : regs_) {
        reg = 0;
    }
    
    // Initialize CSRs
    for (auto& csr : csrs_) {
        csr = 0;
    }
    
    // Set default CSR values
    // ...
    
    // Initialize pipeline registers
    for (auto& reg : pipeline_regs) {
        reg = PipelineRegister();
    }
}

void CPU::enable_pipelining(bool enable) {
    pipelining_enabled_ = enable;
    if (!enable) {
        // Flush pipeline when disabling
        flush_pipeline();
    }
}

bool CPU::is_pipelining_enabled() const {
    return pipelining_enabled_;
}

void CPU::step() {
    if (!pipelining_enabled_) {
        // Non-pipelined mode (original implementation)
        reg_t instr = fetch();
        execute(instr);
        pc_ += 4;
        return;
    }
    
    // Pipeline mode
    // Handle stalls first
    if (stall_next_cycle_) {
        stall_next_cycle_ = false;
        // Insert bubble in fetch stage
        pipeline_regs[STAGE_FETCH].valid = false;
        return;
    }
    
    // Handle pipeline flush (e.g., after branch misprediction)
    if (pipeline_flush_) {
        flush_pipeline();
        pipeline_flush_ = false;
    }
    
    // ====================
    // 5-stage pipeline execution
    // ====================
    
    // Stage 5: Writeback (W)
    if (pipeline_regs[STAGE_WRITEBACK - 1].valid) {
        writeback(pipeline_regs[STAGE_WRITEBACK - 1]);
    }
    
    // Stage 4: Memory (M) -> Writeback (W)
    if (pipeline_regs[STAGE_MEMORY - 1].valid) {
        memory_access(pipeline_regs[STAGE_MEMORY - 1]);
        pipeline_regs[STAGE_WRITEBACK - 1] = pipeline_regs[STAGE_MEMORY - 1];
    } else {
        pipeline_regs[STAGE_WRITEBACK - 1].valid = false;
    }
    
    // Stage 3: Execute (E) -> Memory (M)
    if (pipeline_regs[STAGE_EXECUTE - 1].valid) {
        execute_instruction(pipeline_regs[STAGE_EXECUTE - 1]);
        pipeline_regs[STAGE_MEMORY - 1] = pipeline_regs[STAGE_EXECUTE - 1];
    } else {
        pipeline_regs[STAGE_MEMORY - 1].valid = false;
    }
    
    // Stage 2: Decode (D) -> Execute (E)
    if (pipeline_regs[STAGE_DECODE - 1].valid) {
        // Check for data hazards
        if (detect_data_hazard(pipeline_regs[STAGE_DECODE - 1])) {
            // Stall pipeline
            stall_pipeline();
            return;
        }
        
        // Forward data if needed
        forward_data(pipeline_regs[STAGE_DECODE - 1]);
        
        decode_instruction(pipeline_regs[STAGE_DECODE - 1]);
        pipeline_regs[STAGE_EXECUTE - 1] = pipeline_regs[STAGE_DECODE - 1];
    } else {
        pipeline_regs[STAGE_EXECUTE - 1].valid = false;
    }
    
    // Stage 1: Fetch (F) -> Decode (D)
    // Check for control hazards (branches in pipeline)
    if (detect_control_hazard()) {
        // Simple solution: stall fetch until branch resolved
        // In real CPU, would use branch prediction
        pipeline_regs[STAGE_DECODE - 1].valid = false;
        stall_next_cycle_ = true;
    } else {
        PipelineRegister& fetch_reg = pipeline_regs[STAGE_DECODE - 1];
        fetch_reg.valid = true;
        fetch_reg.pc = pc_;
        fetch_reg.instruction = fetch_instruction(pc_);
        
        // Increment PC for next instruction
        pc_ += 4;
    }
}

void CPU::run(uint64_t steps) {
    if (steps == 0) {
        // Run forever (until exception or break)
        while (true) {
            step();
        }
    } else {
        for (uint64_t i = 0; i < steps; ++i) {
            step();
        }
    }
}

void CPU::reset() {
    pc_ = 0x80000000;
    for (auto& reg : regs_) {
        reg = 0;
    }
    regs_[2] = 0x80000000; // Stack pointer
    
    // Reset pipeline
    flush_pipeline();
    stall_next_cycle_ = false;
    pipeline_flush_ = false;
}

// ====================
// Pipeline stage implementations
// ====================

reg_t CPU::fetch_instruction(addr_t pc) {
    uint32_t instr;
    if (!bus_->read(pc, 4, reinterpret_cast<uint8_t*>(&instr))) {
        handle_exception(1, pc); // Instruction access fault
        return 0;
    }
    return instr;
}

void CPU::decode_instruction(PipelineRegister& reg) {
    // Extract basic instruction fields
    reg_t instr = reg.instruction;
    reg.opcode = instr & 0x7F;
    reg.rd = (instr >> 7) & 0x1F;
    reg.rs1 = (instr >> 15) & 0x1F;
    reg.rs2 = (instr >> 20) & 0x1F;
    
    // Read register values
    reg.rs1_val = read_register(reg.rs1);
    reg.rs2_val = read_register(reg.rs2);
    
    // Decode immediate value (simplified - full implementation needed)
    // This is a placeholder - real implementation would decode based on instruction type
    reg.decoded_imm = (instr >> 20) & 0xFFF;
    
    // Determine if this instruction writes back to register file
    // R-type, I-type, U-type, J-type instructions write to rd (except for some like stores)
    reg.needs_writeback = (reg.rd != 0) && (reg.opcode != 0x23); // Not a store
    
    // Determine if this is a branch/jump instruction
    reg.is_branch = (reg.opcode == 0x63) || (reg.opcode == 0x6F) || (reg.opcode == 0x67);
}

void CPU::execute_instruction(PipelineRegister& reg) {
    // Placeholder for ALU execution
    // Real implementation would decode and execute based on opcode/funct3/funct7
    
    if (reg.is_branch) {
        // Simple branch evaluation (always not taken for now)
        // Real implementation would evaluate condition
        reg.branch_taken = false;
        reg.branch_target = reg.pc + reg.decoded_imm;
        
        if (reg.branch_taken) {
            // Branch taken - need to flush pipeline
            pipeline_flush_ = true;
            pc_ = reg.branch_target;
        }
    }
    
    // Simple ALU operation for demonstration
    switch (reg.opcode) {
        case 0x13: // I-type ALU
            reg.alu_result = reg.rs1_val + reg.decoded_imm;
            break;
        case 0x33: // R-type ALU
            reg.alu_result = reg.rs1_val + reg.rs2_val;
            break;
        default:
            reg.alu_result = 0;
            break;
    }
}

void CPU::memory_access(PipelineRegister& reg) {
    // Placeholder for memory operations
    // Real implementation would handle loads/stores
    
    // For now, just pass ALU result through
    reg.mem_result = reg.alu_result;
}

void CPU::writeback(PipelineRegister& reg) {
    if (reg.valid && reg.needs_writeback && reg.rd != 0) {
        write_register(reg.rd, reg.mem_result);
    }
}

// ====================
// Hazard detection and resolution
// ====================

bool CPU::detect_data_hazard(const PipelineRegister& decode_stage) const {
    if (!decode_stage.valid) return false;
    
    // Check if decode stage needs rs1 or rs2 that will be written by instructions in pipeline
    for (int stage = STAGE_EXECUTE - 1; stage <= STAGE_WRITEBACK - 1; stage++) {
        if (pipeline_regs[stage].valid && pipeline_regs[stage].needs_writeback) {
            if (decode_stage.rs1 != 0 && decode_stage.rs1 == pipeline_regs[stage].rd) {
                return true; // RAW hazard on rs1
            }
            if (decode_stage.rs2 != 0 && decode_stage.rs2 == pipeline_regs[stage].rd) {
                return true; // RAW hazard on rs2
            }
        }
    }
    
    return false;
}

bool CPU::detect_control_hazard() const {
    // Check if any instruction in pipeline is a branch
    for (int stage = STAGE_DECODE - 1; stage <= STAGE_EXECUTE - 1; stage++) {
        if (pipeline_regs[stage].valid && pipeline_regs[stage].is_branch) {
            return true;
        }
    }
    return false;
}

void CPU::forward_data(PipelineRegister& decode_stage) {
    // Forward data from later stages to decode stage if needed
    for (int stage = STAGE_EXECUTE - 1; stage <= STAGE_WRITEBACK - 1; stage++) {
        if (pipeline_regs[stage].valid && pipeline_regs[stage].needs_writeback) {
            if (decode_stage.rs1 != 0 && decode_stage.rs1 == pipeline_regs[stage].rd) {
                // Forward from appropriate stage
                if (stage == STAGE_EXECUTE - 1) {
                    decode_stage.rs1_val = pipeline_regs[stage].alu_result;
                } else if (stage == STAGE_MEMORY - 1) {
                    decode_stage.rs1_val = pipeline_regs[stage].mem_result;
                } else if (stage == STAGE_WRITEBACK - 1) {
                    decode_stage.rs1_val = read_register(pipeline_regs[stage].rd);
                }
            }
            if (decode_stage.rs2 != 0 && decode_stage.rs2 == pipeline_regs[stage].rd) {
                // Forward from appropriate stage
                if (stage == STAGE_EXECUTE - 1) {
                    decode_stage.rs2_val = pipeline_regs[stage].alu_result;
                } else if (stage == STAGE_MEMORY - 1) {
                    decode_stage.rs2_val = pipeline_regs[stage].mem_result;
                } else if (stage == STAGE_WRITEBACK - 1) {
                    decode_stage.rs2_val = read_register(pipeline_regs[stage].rd);
                }
            }
        }
    }
}

void CPU::stall_pipeline() {
    stall_next_cycle_ = true;
    // Invalidate the decode stage (insert bubble)
    pipeline_regs[STAGE_DECODE - 1].valid = false;
}

void CPU::flush_pipeline() {
    // Invalidate all pipeline registers
    for (auto& reg : pipeline_regs) {
        reg.valid = false;
    }
}

// ====================
// Utility methods
// ====================

reg_t CPU::read_register(uint8_t reg) const {
    if (reg >= 32) return 0;
    return regs_[reg];
}

void CPU::write_register(uint8_t reg, reg_t value) {
    if (reg >= 32) return;
    regs_[reg] = value;
}

// ====================
// Original non-pipelined methods (for compatibility)
// ====================

reg_t CPU::fetch() {
    return fetch_instruction(pc_);
}

void CPU::execute(reg_t instr) {
    // TODO: Implement instruction decoding and execution
    std::cout << "Executing instruction: 0x" << std::hex << instr 
              << " at PC: 0x" << pc_ << std::dec << std::endl;
}

void CPU::handle_exception(uint32_t cause, addr_t tval) {
    std::cerr << "Exception " << cause << " at PC 0x" << std::hex << pc_
              << " tval 0x" << tval << std::dec << std::endl;
    // TODO: Implement proper exception handling
    exit(1);
}

// ====================
// Public interface methods
// ====================

reg_t CPU::get_reg(uint8_t reg) const {
    return read_register(reg);
}

void CPU::set_reg(uint8_t reg, reg_t value) {
    write_register(reg, value);
}

addr_t CPU::get_pc() const {
    return pc_;
}

void CPU::set_pc(addr_t pc) {
    pc_ = pc;
}

reg_t CPU::get_csr(addr_t csr) const {
    if (csr >= 4096) return 0;
    return csrs_[csr];
}

void CPU::set_csr(addr_t csr, reg_t value) {
    if (csr >= 4096) return;
    csrs_[csr] = value;
}

void CPU::dump_registers() const {
    std::cout << "Registers:" << std::endl;
    for (int i = 0; i < 32; ++i) {
        std::cout << "  x" << i << ": 0x" << std::hex << regs_[i] << std::dec;
        if (i % 4 == 3) std::cout << std::endl;
        else std::cout << "  ";
    }
}

void CPU::dump_csrs() const {
    std::cout << "CSRs:" << std::endl;
    // Print only non-zero CSRs for brevity
    for (int i = 0; i < 4096; ++i) {
        if (csrs_[i] != 0) {
            std::cout << "  csr[" << i << "]: 0x" << std::hex << csrs_[i] << std::dec << std::endl;
        }
    }
}

void CPU::dump_pipeline() const {
    std::cout << "Pipeline state:" << std::endl;
    std::cout << "  Pipelining enabled: " << (pipelining_enabled_ ? "yes" : "no") << std::endl;
    
    const char* stage_names[] = {"F-D", "D-E", "E-M", "M-W"};
    
    for (int i = 0; i < STAGE_COUNT - 1; ++i) {
        std::cout << "  Stage " << stage_names[i] << ": ";
        if (pipeline_regs[i].valid) {
            std::cout << "valid, PC=0x" << std::hex << pipeline_regs[i].pc
                      << ", instr=0x" << pipeline_regs[i].instruction << std::dec;
            if (pipeline_regs[i].rd != 0) {
                std::cout << ", rd=x" << (int)pipeline_regs[i].rd;
            }
        } else {
            std::cout << "invalid";
        }
        std::cout << std::endl;
    }
}