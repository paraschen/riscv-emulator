#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <vector>
#include <memory>
#include "types.h"
#include "device.h"

class Memory : public Device {
public:
    Memory(size_t size);
    ~Memory() = default;
    
    // Device interface
    bool read(addr_t addr, size_t len, uint8_t* data) override;
    bool write(addr_t addr, size_t len, const uint8_t* data) override;
    
    // Direct access
    uint8_t* data();
    const uint8_t* data() const;
    size_t size() const;
    
    // Load binary data
    bool load(addr_t addr, const uint8_t* data, size_t len);
    bool load_file(addr_t addr, const std::string& filename);
    
private:
    std::vector<uint8_t> data_;
};

#endif // MEMORY_H
