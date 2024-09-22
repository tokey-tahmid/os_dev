# Operating System Development

![Operating System](OS.png)

**Operating System Development | University of Tennessee, Knoxville | Aug 2023 – Dec 2023**

Built a fully functional Operating System from scratch as part of the COSC562 - OS Design/Implementation course.

---

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Project Structure](#project-structure)
- [Installation](#installation)
- [Building the OS](#building-the-os)
- [Running the OS](#running-the-os)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)

## Introduction

This project showcases the development of a fully functional Operating System (OS) built from the ground up. Undertaken as part of the COSC562 - OS Design/Implementation course at the University of Tennessee, Knoxville, this OS serves as a comprehensive demonstration of key operating system principles, including process management, memory management, file systems, and system calls.

## Features

- **Process Management:** Creation, scheduling, and termination of processes.
- **Memory Management:** Implementation of virtual memory, paging, and memory allocation.
- **File System:** Basic file system with support for file creation, deletion, reading, and writing.
- **System Calls:** Interface for user-space applications to interact with the OS.
- **User Space:** Support for user-space applications and a basic shell environment.
- **Bootloader:** Custom bootloader to initialize the OS during startup.
- **Interrupt Handling:** Mechanisms to handle hardware and software interrupts.
- **Device Drivers:** Basic drivers for essential hardware components.

## Project Structure

The repository is organized as follows:

```
tokey-tahmid/
├── asm/            # Assembly code for low-level operations
├── deps/           # Dependencies and external libraries
├── lds/            # Linker scripts
├── objs/           # Compiled object files
├── os-userspace/   # User-space OS components
├── sbi/            # SBI (Supervisor Binary Interface) code
├── src/            # Source code for the OS kernel
├── user-src/       # Source code for user-space applications
├── user/           # User-space utilities and programs
├── util/           # Utility scripts and tools
├── .clang-format   # Clang formatting configuration
├── .gitignore      # Git ignore rules
├── .gitmodules     # Git submodules configuration
├── Makefile        # Build configuration
├── OS.png          # Project image
├── compile_flags.txt# Compilation flags
└── elf.c           # ELF (Executable and Linkable Format) handling code
```

### Directory Overview

- **asm/**: Contains assembly language source files for low-level operations and bootstrapping the OS.
- **deps/**: Houses external dependencies and third-party libraries required for the OS.
- **lds/**: Includes linker scripts used during the build process to define memory layout.
- **objs/**: Stores compiled object files generated during the build process.
- **os-userspace/**: Contains components and utilities that operate in user space.
- **sbi/**: Implements the Supervisor Binary Interface for interacting with lower-level firmware or hypervisors.
- **src/**: Main source code for the OS kernel, including core functionalities like process and memory management.
- **user-src/**: Source code for user-space applications that run on the OS.
- **user/**: Pre-compiled user-space utilities and programs.
- **util/**: Utility scripts and tools to aid in development and testing.
- **Makefile**: Defines the build process, compilation rules, and targets.
- **OS.png**: Visual representation of the OS architecture or project logo.
- **compile_flags.txt**: Contains compilation flags and settings used during the build process.
- **elf.c**: Handles ELF file format operations, essential for loading and executing binaries.

## Installation

### Prerequisites

- **Compiler:** GCC or Clang
- **Build Tools:** Make
- **Libraries:** Ensure all dependencies listed in the `deps/` directory are installed.
- **Emulator:** QEMU or another suitable emulator for testing the OS.

### Steps

1. **Clone the Repository**

   ```bash
   git clone https://github.com/tokey-tahmid/os_dev.git
   cd os_dev
   ```

2. **Initialize Submodules**

   If the project uses Git submodules, initialize them:

   ```bash
   git submodule update --init --recursive
   ```

## Building the OS

To build the operating system, use the provided Makefile. This process compiles the source code and generates the necessary binaries.

```bash
make
```

**Make Targets:**

- `make all`: Builds the entire project.
- `make clean`: Cleans the build directory by removing compiled object files.
- `make run`: Builds the project and runs the OS in the emulator.
- `make debug`: Builds the project with debug symbols for use with a debugger.

## Running the OS

After successfully building the OS, you can run it using an emulator like QEMU.

```bash
make run
```

This command will:

1. Compile the OS (if not already built).
2. Launch QEMU with the generated OS binary.
3. Display the OS boot sequence and any user-space applications.

<!-- **Example Output:**

```
Booting OS...
Initializing kernel...
Starting process scheduler...
Launching shell...
>
```

## Contributing

This project was developed as part of an academic course, but contributions for educational purposes are welcome. To contribute:

1. **Fork the Repository**

   Click the "Fork" button at the top right of the repository page to create a personal copy.

2. **Create a New Branch**

   ```bash
   git checkout -b feature/YourFeature
   ```

3. **Commit Your Changes**

   ```bash
   git commit -m "Add your feature"
   ```

4. **Push to the Branch**

   ```bash
   git push origin feature/YourFeature
   ```

5. **Open a Pull Request**

   Navigate to the original repository and create a pull request from your forked repository.

**Please ensure that your contributions adhere to the project's coding standards and include appropriate documentation and tests.**

## License

This project is licensed under the [MIT License](LICENSE).

## Contact

For any questions, issues, or support, please contact:

- **Name:** Tokey Tahmid
- **Email:** [tokey.tahmid@example.com](mailto:tokey.tahmid@example.com)

---

**Acknowledgements**

Special thanks to the instructors and peers at the University of Tennessee, Knoxville, for their guidance and support throughout the development of this operating system. This project leverages several open-source tools and libraries, including [QEMU](https://www.qemu.org/), [GCC](https://gcc.gnu.org/), and [Clang](https://clang.llvm.org/). -->