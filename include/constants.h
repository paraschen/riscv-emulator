#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "types.h"

// Memory addresses
constexpr addr_t DRAM_BASE = 0x80000000;
constexpr addr_t DRAM_SIZE = 128 * 1024 * 1024; // 128 MB

// Device addresses
constexpr addr_t UART_BASE = 0x10000000;
constexpr addr_t CLINT_BASE = 0x02000000;
constexpr addr_t PLIC_BASE = 0x0C000000;

// CSR addresses
constexpr addr_t CSR_MSTATUS = 0x300;
constexpr addr_t CSR_MIE = 0x304;
constexpr addr_t CSR_MTVEC = 0x305;
constexpr addr_t CSR_MEPC = 0x341;
constexpr addr_t CSR_MCAUSE = 0x342;

#endif // CONSTANTS_H
