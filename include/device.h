#ifndef DEVICE_H
#define DEVICE_H

#include "types.h"
#include <cstddef>

class Device {
public:
    virtual ~Device() = default;
    
    // Read/write interface
    virtual bool read(addr_t addr, size_t len, uint8_t* data) = 0;
    virtual bool write(addr_t addr, size_t len, const uint8_t* data) = 0;
    
    // Device properties
    virtual addr_t base() const = 0;
    virtual addr_t size() const = 0;
    
    // Optional: interrupt support
    virtual bool has_interrupt() const { return false; }
    virtual uint32_t get_interrupt() { return 0; }
    virtual void clear_interrupt() {}
    
    // Optional: clock/tick support
    virtual void tick(uint64_t cycles) {}
};

#endif // DEVICE_H
