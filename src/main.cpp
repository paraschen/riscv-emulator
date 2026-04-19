#include <iostream>
#include <string>
#include <memory>
#include "cpu.h"
#include "memory.h"
#include "bus.h"
#include "elf_loader.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <elf_file>" << std::endl;
        return 1;
    }

    std::string elf_path = argv[1];
    
    try {
        // Create system bus
        auto bus = std::make_shared<Bus>();
        
        // Create memory (128 MB)
        auto memory = std::make_shared<Memory>(128 * 1024 * 1024);
        bus->add_device(0x80000000, memory);
        
        // Create CPU
        auto cpu = std::make_unique<CPU>(bus);
        
        // Load ELF file
        ElfLoader loader;
        loader.load(elf_path, memory);
        
        // Set PC to entry point
        cpu->set_pc(loader.get_entry_point());
        
        std::cout << "Starting RISC-V emulation..." << std::endl;
        std::cout << "Entry point: 0x" << std::hex << loader.get_entry_point() << std::endl;
        
        // Main execution loop
        while (true) {
            cpu->step();
            // Add breakpoint/debug checks here
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
