#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <memory>
#include "types.h"
#include "bus.h"

// Pipeline stages
enum PipelineStage {
    STAGE_FETCH = 0,
    STAGE_DECODE,
    STAGE_EXECUTE,
    STAGE_MEMORY,
    STAGE_WRITEBACK,
    STAGE_COUNT
};

// Pipeline register between stages
struct PipelineRegister {
    bool valid;             // Whether this register contains valid data
    addr_t pc;              // Program counter at fetch stage
    reg_t instruction;      // Instruction word
    reg_t decoded_imm;      // Decoded immediate value
    uint8_t opcode;         // Instruction opcode
    uint8_t rd;             // Destination register
    uint8_t rs1;            // Source register 1
    uint8_t rs2;            // Source register 2
    reg_t rs1_val;          // Value of rs1
    reg_t rs2_val;          // Value of rs2
    reg_t alu_result;       // Result from execute stage
    reg_t mem_result;       // Result from memory stage
    bool needs_writeback;   // Whether this instruction writes to register file
    bool is_branch;         // Whether this is a branch/jump instruction
    bool branch_taken;      // Whether branch was taken
    addr_t branch_target;   // Target address if branch taken
    
    PipelineRegister() : valid(false), pc(0), instruction(0), decoded_imm(0),
                         opcode(0), rd(0), rs1(0), rs2(0), rs1_val(0), rs2_val(0),
                         alu_result(0), mem_result(0), needs_writeback(false),
                         is_branch(false), branch_taken(false), branch_target(0) {}
};

class CPU {
public:
    CPU(std::shared_ptr<Bus> bus);
    ~CPU() = default;
    
    // Core execution
    void step();
    void run(uint64_t steps = 0);
    void reset();
    
    // Pipeline control
    void enable_pipelining(bool enable);
    bool is_pipelining_enabled() const;
    
    // Register access
    reg_t get_reg(uint8_t reg) const;
    void set_reg(uint8_t reg, reg_t value);
    
    // PC access
    addr_t get_pc() const;
    void set_pc(addr_t pc);
    
    // CSR access
    reg_t get_csr(addr_t csr) const;
    void set_csr(addr_t csr, reg_t value);
    
    // Debug
    void dump_registers() const;
    void dump_csrs() const;
    void dump_pipeline() const;

private:
    std::shared_ptr<Bus> bus_;
    reg_t regs_[32];        // General purpose registers
    addr_t pc_;             // Program counter (fetch stage)
    reg_t csrs_[4096];      // Control and status registers
    
    // Pipeline state
    bool pipelining_enabled_;
    PipelineRegister pipeline_regs[STAGE_COUNT - 1]; // F-D, D-E, E-M, M-W
    
    // Branch prediction state
    bool stall_next_cycle_;
    bool pipeline_flush_;
    
    // Internal helpers
    reg_t fetch_instruction(addr_t pc);
    void decode_instruction(PipelineRegister& reg);
    void execute_instruction(PipelineRegister& reg);
    void memory_access(PipelineRegister& reg);
    void writeback(PipelineRegister& reg);
    
    // Hazard detection and resolution
    bool detect_data_hazard(const PipelineRegister& decode_stage) const;
    bool detect_control_hazard() const;
    void forward_data(PipelineRegister& decode_stage);
    void stall_pipeline();
    void flush_pipeline();
    
    // Original non-pipelined methods (for compatibility/fallback)
    reg_t fetch();
    void execute(reg_t instr);
    void handle_exception(uint32_t cause, addr_t tval = 0);
    
    // Utility
    reg_t read_register(uint8_t reg) const;
    void write_register(uint8_t reg, reg_t value);
};

#endif // CPU_H
