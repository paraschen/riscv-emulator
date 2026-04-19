#ifndef BUS_H
#define BUS_H

#include <map>
#include <memory>
#include <vector>
#include "types.h"
#include "device.h"

struct DeviceMapping {
    addr_t base;
    addr_t size;
    std::shared_ptr<Device> device;
};

class Bus {
public:
    Bus();
    ~Bus() = default;
    
    // Device management
    void add_device(addr_t base, std::shared_ptr<Device> device);
    void remove_device(addr_t base);
    std::shared_ptr<Device> find_device(addr_t addr, addr_t* offset = nullptr);
    
    // Read/write interface
    bool read(addr_t addr, size_t len, uint8_t* data);
    bool write(addr_t addr, size_t len, const uint8_t* data);
    
    // Typed reads
    uint8_t read8(addr_t addr);
    uint16_t read16(addr_t addr);
    uint32_t read32(addr_t addr);
    uint64_t read64(addr_t addr);
    
    // Typed writes
    void write8(addr_t addr, uint8_t value);
    void write16(addr_t addr, uint16_t value);
    void write32(addr_t addr, uint32_t value);
    void write64(addr_t addr, uint64_t value);
    
    // Debug
    void dump_devices() const;
    
private:
    std::vector<DeviceMapping> devices_;
};

#endif // BUS_H
