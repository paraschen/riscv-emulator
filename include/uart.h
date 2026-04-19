#ifndef UART_H
#define UART_H

#include "device.h"

class UART : public Device {
public:
    UART(addr_t base);
    ~UART() = default;
    
    // Device interface
    bool read(addr_t addr, size_t len, uint8_t* data) override;
    bool write(addr_t addr, size_t len, const uint8_t* data) override;
    
    addr_t base() const override;
    addr_t size() const override;
    
    // UART specific
    void set_stdin_callback(std::function<void(uint8_t)> callback);
    void putc(char c);
    char getc();
    
private:
    addr_t base_;
    std::vector<uint8_t> rx_buffer_;
    std::vector<uint8_t> tx_buffer_;
    
    // Registers
    uint32_t data_reg_;
    uint32_t status_reg_;
    uint32_t control_reg_;
    
    std::function<void(uint8_t)> stdin_callback_;
};

#endif // UART_H
