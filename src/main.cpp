#include <iostream>
#include <string>
#include <memory>
#include <cstring>
#include "cpu.h"
#include "memory.h"
#include "bus.h"

// Simple program to test pipeline
const uint32_t test_program[] = {
    0x00100093, // addi x1, x0, 1   # x1 = 1
    0x00200113, // addi x2, x0, 2   # x2 = 2
    0x002081b3, // add x3, x1, x2   # x3 = x1 + x2 = 3
    0x00308233, // add x4, x1, x3   # x4 = x1 + x3 = 4
    0x004102b3, // add x5, x2, x4   # x5 = x2 + x4 = 6
    0x00000013, // nop
    0x00000013, // nop
    0x00000013, // nop
    0x00000013, // nop
    0x00000013  // nop
};

const int PROGRAM_SIZE = sizeof(test_program) / sizeof(test_program[0]);

void load_program(std::shared_ptr<Bus> bus) {
    const addr_t ENTRY_POINT = 0x80000000;
    // Write program through bus
    for (size_t i = 0; i < PROGRAM_SIZE; ++i) {
        uint32_t instruction = test_program[i];
        bool success = bus->write(ENTRY_POINT + i * 4, 4, reinterpret_cast<const uint8_t*>(&instruction));
        if (!success) {
            std::cerr << "Failed to write instruction " << i << " to memory!" << std::endl;
            exit(1);
        }
    }
    std::cout << "Program written successfully to 0x" << std::hex << ENTRY_POINT << std::dec << std::endl;
}

void run_non_pipelined(std::shared_ptr<Bus> bus, std::unique_ptr<CPU>& cpu) {
    std::cout << "=== Running in NON-PIPELINED mode ===" << std::endl;
    
    cpu->enable_pipelining(false);
    cpu->reset();
    
    // Run program
    for (int i = 0; i < PROGRAM_SIZE; ++i) {
        std::cout << "Step " << i << ":" << std::endl;
        cpu->step();
        cpu->dump_pipeline();
        std::cout << "PC = 0x" << std::hex << cpu->get_pc() << std::dec << std::endl;
        cpu->dump_registers();
        std::cout << std::endl;
    }
}

void run_pipelined(std::shared_ptr<Bus> bus, std::unique_ptr<CPU>& cpu) {
    std::cout << "=== Running in PIPELINED mode ===" << std::endl;
    
    cpu->enable_pipelining(true);
    cpu->reset();
    
    // Run enough steps to fill pipeline and complete all instructions
    int total_steps = PROGRAM_SIZE + 4; // Pipeline depth
    
    for (int i = 0; i < total_steps; ++i) {
        std::cout << "Cycle " << i << ":" << std::endl;
        cpu->step();
        cpu->dump_pipeline();
        
        // Show register values after each cycle
        std::cout << "Registers after cycle " << i << ":" << std::endl;
        for (int reg = 1; reg <= 5; ++reg) {
            std::cout << "  x" << reg << ": 0x" << std::hex << cpu->get_reg(reg) << std::dec;
            if (reg % 5 == 0) std::cout << std::endl;
            else std::cout << "  ";
        }
        std::cout << std::endl;
    }
}

void compare_results(std::unique_ptr<CPU>& cpu1, std::unique_ptr<CPU>& cpu2) {
    std::cout << "=== Comparing results ===" << std::endl;
    
    bool match = true;
    for (int reg = 0; reg < 32; ++reg) {
        reg_t val1 = cpu1->get_reg(reg);
        reg_t val2 = cpu2->get_reg(reg);
        if (val1 != val2) {
            std::cout << "Mismatch in x" << reg << ": non-pipelined=0x" << std::hex << val1 
                      << ", pipelined=0x" << val2 << std::dec << std::endl;
            match = false;
        }
    }
    
    if (match) {
        std::cout << "All register values match!" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    try {
        bool test_pipelined = true;
        bool test_non_pipelined = true;
        
        // Parse command line arguments
        for (int i = 1; i < argc; ++i) {
            if (strcmp(argv[i], "--pipelined-only") == 0) {
                test_non_pipelined = false;
            } else if (strcmp(argv[i], "--non-pipelined-only") == 0) {
                test_pipelined = false;
            } else if (strcmp(argv[i], "--help") == 0) {
                std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
                std::cout << "Options:" << std::endl;
                std::cout << "  --pipelined-only     Test only pipelined mode" << std::endl;
                std::cout << "  --non-pipelined-only Test only non-pipelined mode" << std::endl;
                std::cout << "  --help               Show this help message" << std::endl;
                return 0;
            }
        }
        
        // Create system bus
        auto bus = std::make_shared<Bus>();
        
        // Create memory (128 MB)
        auto memory = std::make_shared<Memory>(128 * 1024 * 1024);
        std::cout << "Memory created with size: " << memory->size() << " bytes (0x" << std::hex << memory->size() << std::dec << ")" << std::endl;
        bus->add_device(0x80000000, memory);
        
        // Load test program
        load_program(bus);
        
        std::cout << "RISC-V Emulator with Pipeline Support" << std::endl;
        std::cout << "======================================" << std::endl;
        std::cout << "Program loaded at 0x80000000 (" << PROGRAM_SIZE << " instructions)" << std::endl;
        std::cout << std::endl;
        
        std::unique_ptr<CPU> cpu_non_pipelined;
        std::unique_ptr<CPU> cpu_pipelined;
        
        if (test_non_pipelined) {
            cpu_non_pipelined = std::make_unique<CPU>(bus);
            run_non_pipelined(bus, cpu_non_pipelined);
        }
        
        if (test_pipelined) {
            cpu_pipelined = std::make_unique<CPU>(bus);
            run_pipelined(bus, cpu_pipelined);
        }
        
        // Compare results if both modes were tested
        if (test_non_pipelined && test_pipelined) {
            compare_results(cpu_non_pipelined, cpu_pipelined);
        }
        
        std::cout << "Test completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}