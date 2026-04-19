#include "bus.h"
#include <iostream>

Bus::Bus() = default;

void Bus::add_device(addr_t base, std::shared_ptr<Device> device) {
    DeviceMapping mapping;
    mapping.base = base;
    mapping.size = device->size();
    mapping.device = device;
    devices_.push_back(mapping);
    
    std::cout << "Added device at 0x" << std::hex << base 
              << " size 0x" << mapping.size << std::dec << std::endl;
}

void Bus::remove_device(addr_t base) {
    for (auto it = devices_.begin(); it != devices_.end(); ++it) {
        if (it->base == base) {
            devices_.erase(it);
            return;
        }
    }
}

std::shared_ptr<Device> Bus::find_device(addr_t addr, addr_t* offset) {
    for (const auto& mapping : devices_) {
        if (addr >= mapping.base && addr < mapping.base + mapping.size) {
            if (offset) {
                *offset = addr - mapping.base;
            }
            return mapping.device;
        }
    }
    return nullptr;
}

bool Bus::read(addr_t addr, size_t len, uint8_t* data) {
    addr_t offset;
    auto device = find_device(addr, &offset);
    if (!device) {
        return false;
    }
    return device->read(offset, len, data);
}

bool Bus::write(addr_t addr, size_t len, const uint8_t* data) {
    addr_t offset;
    auto device = find_device(addr, &offset);
    if (!device) {
        return false;
    }
    return device->write(offset, len, data);
}

uint8_t Bus::read8(addr_t addr) {
    uint8_t value;
    if (!read(addr, 1, &value)) {
        return 0;
    }
    return value;
}

uint16_t Bus::read16(addr_t addr) {
    uint16_t value;
    if (!read(addr, 2, reinterpret_cast<uint8_t*>(&value))) {
        return 0;
    }
    return value;
}

uint32_t Bus::read32(addr_t addr) {
    uint32_t value;
    if (!read(addr, 4, reinterpret_cast<uint8_t*>(&value))) {
        return 0;
    }
    return value;
}

uint64_t Bus::read64(addr_t addr) {
    uint64_t value;
    if (!read(addr, 8, reinterpret_cast<uint8_t*>(&value))) {
        return 0;
    }
    return value;
}

void Bus::write8(addr_t addr, uint8_t value) {
    write(addr, 1, &value);
}

void Bus::write16(addr_t addr, uint16_t value) {
    write(addr, 2, reinterpret_cast<const uint8_t*>(&value));
}

void Bus::write32(addr_t addr, uint32_t value) {
    write(addr, 4, reinterpret_cast<const uint8_t*>(&value));
}

void Bus::write64(addr_t addr, uint64_t value) {
    write(addr, 8, reinterpret_cast<const uint8_t*>(&value));
}

void Bus::dump_devices() const {
    std::cout << "Bus devices:" << std::endl;
    for (const auto& mapping : devices_) {
        std::cout << "  0x" << std::hex << mapping.base 
                  << " - 0x" << mapping.base + mapping.size
                  << " (size 0x" << mapping.size << ")" << std::dec << std::endl;
    }
}
