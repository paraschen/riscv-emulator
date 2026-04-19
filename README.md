# RISC-V Emulator - Complete System Simulation

A complete RISC-V system emulator written in C++, supporting the RV32IMAFD instruction set, memory management, peripherals, and full system simulation.

## Features
- RV32IMAFD instruction set support
- Memory management unit (MMU)
- Device/peripheral simulation (UART, CLINT, PLIC)
- ELF file loading and execution
- Debugger interface
- Configurable system parameters

## Build Instructions

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

```bash
./riscv-emulator <path_to_elf_file>
```

## Project Structure

- `src/` - Core emulator source files
- `include/` - Public headers
- `tests/` - Unit and integration tests
- `examples/` - Example programs and tests
- `docs/` - Documentation
- `build/` - Build artifacts (gitignored)

## License

MIT
