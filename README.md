# RISC-V Emulator - Complete System Simulation

A complete RISC-V system emulator written in C++, supporting the RV32IMAFD instruction set, memory management, peripherals, and full system simulation.

## Features
- RV32IMAFD instruction set support
- Memory management unit (MMU)
- Device/peripheral simulation (UART, CLINT, PLIC)
- ELF file loading and execution
- Debugger interface
- Configurable system parameters

## Prerequisites
- C++17 compatible compiler (g++ 8+, clang++ 10+)
- CMake 3.16+
- Git

## Build Instructions

### Using CMake (Recommended)
```bash
mkdir build
cd build
cmake ..
make
```

### Using Makefile
```bash
make release   # for optimized build
make debug     # for debug build with symbols
```

## Usage

```bash
./riscv-emulator <path_to_elf_file>
```

## Project Structure

- `src/` - Core emulator source files
  - `cpu.cpp` - CPU core implementation
  - `memory.cpp` - Memory subsystem
  - `bus.cpp` - Device bus and MMIO
  - `device.cpp` - Base device class
- `include/` - Public headers
  - `cpu.h` - CPU interface
  - `memory.h` - Memory interface
  - `bus.h` - Bus interface
  - `device.h` - Device base class
  - `uart.h` - UART device
  - `elf_loader.h` - ELF file loader
  - `types.h`, `constants.h` - Common types and constants
- `tests/` - Unit and integration tests
- `examples/` - Example programs and tests
- `docs/` - Documentation
- `build/` - Build artifacts (gitignored)

## Development Status

This is a work-in-progress project with the following components implemented:

✅ **Project structure and build system** (CMake, Makefile)
✅ **Core class interfaces** (CPU, Memory, Bus, Device)
✅ **Basic implementation stubs** for core components
✅ **Git repository initialized**

🚧 **To be implemented**:
- Instruction decoding and execution
- RISC-V RV32IMAFD instruction set
- Device implementations (UART, CLINT, PLIC)
- ELF loader implementation
- Test suite

## Setting Up GitHub Repository

The project is ready for GitHub. To publish:

1. **Create a repository on GitHub**:
   - Go to https://github.com/new
   - Repository name: `riscv-emulator`
   - Description: "Complete RISC-V system emulator in C++"
   - Public repository
   - **Do NOT** initialize with README, .gitignore, or license
   - Click "Create repository"

2. **Push your code**:
   ```bash
   # If remote is not set:
   git remote add origin https://github.com/paraschen/riscv-emulator.git
   
   # Push to GitHub (use your token as password when prompted):
   git push -u origin main
   ```

   Or use token-embedded URL:
   ```bash
   git push https://@github.com/paraschen/riscv-emulator.git main
   ```

3. **Alternative**: Run the setup script:
   ```bash
   chmod +x setup-github.sh
   ./setup-github.sh
   ```

## Next Steps for Development

1. **Implement CPU instruction decoding** - Start with basic RV32I instructions
2. **Complete memory and bus implementations** - Add proper error handling
3. **Implement device classes** - UART for console I/O, CLINT for timer, PLIC for interrupts
4. **Add ELF loader** - Load and execute RISC-V binaries
5. **Write tests** - Unit tests for each component
6. **Add debugger interface** - Step execution, breakpoints, register inspection

## Testing

```bash
# Build and run tests
mkdir build && cd build
cmake .. -DBUILD_TESTS=ON
make
ctest
```

## License

MIT License - see LICENSE file for details

## Contributing

1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Add tests
5. Submit a pull request