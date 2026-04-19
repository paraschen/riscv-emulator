#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <memory>
#include "types.h"
#include "bus.h"

class CPU {
public:
    CPU(std::shared_ptr<Bus> bus);
    ~CPU() = default;
    
    // Core execution
    void step();
    void run(uint64_t steps = 0);
    void reset();
    
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
    
private:
    std::shared_ptr<Bus> bus_;
    reg_t regs_[32];        // General purpose registers
    addr_t pc_;             // Program counter
    reg_t csrs_[4096];      // Control and status registers
    
    // Internal helpers
    reg_t fetch();
    void execute(reg_t instr);
    void handle_exception(uint32_t cause, addr_t tval = 0);
};

#endif // CPU_H
