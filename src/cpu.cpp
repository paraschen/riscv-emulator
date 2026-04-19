#include "cpu.h"
#include "bus.h"
#include <iostream>

CPU::CPU(std::shared_ptr<Bus> bus) : bus_(bus), pc_(0x80000000) {
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
}

void CPU::step() {
    // Fetch instruction
    reg_t instr = fetch();
    
    // Execute
    execute(instr);
    
    // Increment PC (will be modified by jumps/branches)
    pc_ += 4;
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
}

reg_t CPU::get_reg(uint8_t reg) const {
    if (reg >= 32) return 0;
    return regs_[reg];
}

void CPU::set_reg(uint8_t reg, reg_t value) {
    if (reg >= 32) return;
    regs_[reg] = value;
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

reg_t CPU::fetch() {
    uint32_t instr;
    if (!bus_->read(pc_, 4, reinterpret_cast<uint8_t*>(&instr))) {
        // Handle fetch exception
        handle_exception(1, pc_); // Instruction access fault
        return 0;
    }
    return instr;
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
