#include <iostream>
#include <string>
#include <memory>
#include "cpu.h"
#include "memory.h"
#include "bus.h"

int main(int argc, char* argv[]) {
    try {
        // Create system bus
        auto bus = std::make_shared<Bus>();
        
        // Create memory (128 MB)
        auto memory = std::make_shared<Memory>(128 * 1024 * 1024);
        bus->add_device(0x80000000, memory);
        
        // Create CPU
        auto cpu = std::make_unique<CPU>(bus);
        
        // Set PC to standard RISC-V entry point
        constexpr addr_t ENTRY_POINT = 0x80000000;
        cpu->set_pc(ENTRY_POINT);
        
        std::cout << "Starting RISC-V emulation..." << std::endl;
        std::cout << "Entry point: 0x" << std::hex << ENTRY_POINT << std::dec << std::endl;
        std::cout << "Note: This is a stub implementation. Instructions are not executed yet." << std::endl;
        
        // Main execution loop - limited to 10 steps for demo
        for (int i = 0; i < 10; ++i) {
            cpu->step();
            std::cout << "Step " << i << ": PC = 0x" << std::hex << cpu->get_pc() << std::dec << std::endl;
        }
        
        std::cout << "Demo completed. CPU registers:" << std::endl;
        cpu->dump_registers();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
