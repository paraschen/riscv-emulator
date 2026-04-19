#include "memory.h"
#include <fstream>
#include <iostream>

Memory::Memory(size_t size) : data_(size, 0) {}

bool Memory::read(addr_t addr, size_t len, uint8_t* data) {
    if (addr + len > data_.size()) {
        return false;
    }
    
    for (size_t i = 0; i < len; ++i) {
        data[i] = data_[addr + i];
    }
    return true;
}

bool Memory::write(addr_t addr, size_t len, const uint8_t* data) {
    if (addr + len > data_.size()) {
        return false;
    }
    
    for (size_t i = 0; i < len; ++i) {
        data_[addr + i] = data[i];
    }
    return true;
}

addr_t Memory::base() const {
    return 0;  // Base address is handled by bus mapping
}

addr_t Memory::size() const {
    return static_cast<addr_t>(data_.size());
}

uint8_t* Memory::data() {
    return data_.data();
}

const uint8_t* Memory::data() const {
    return data_.data();
}

bool Memory::load(addr_t addr, const uint8_t* data, size_t len) {
    return write(addr, len, data);
}

bool Memory::load_file(addr_t addr, const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        return false;
    }
    
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    if (addr + size > data_.size()) {
        return false;
    }
    
    file.read(reinterpret_cast<char*>(&data_[addr]), size);
    return !file.fail();
}