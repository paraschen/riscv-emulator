#ifndef ELF_LOADER_H
#define ELF_LOADER_H

#include <string>
#include <memory>
#include "types.h"

class Memory;

class ElfLoader {
public:
    ElfLoader();
    ~ElfLoader();
    
    // Load ELF file into memory
    bool load(const std::string& filename, std::shared_ptr<Memory> memory);
    
    // Get entry point
    addr_t get_entry_point() const;
    
    // Get loaded segments
    const std::vector<std::pair<addr_t, size_t>>& get_segments() const;
    
    // Debug
    void dump_info() const;
    
private:
    addr_t entry_point_;
    std::vector<std::pair<addr_t, size_t>> segments_;
    
    // Internal implementation
    class Impl;
    std::unique_ptr<Impl> impl_;
};

#endif // ELF_LOADER_H
