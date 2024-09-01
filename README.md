# BinaryView

BinaryView is a WIP binary file explorer created to help me learn about different executable formats. Currently, it supports the Portable Executable (PE) format with the goal of expanding to support other formats such as ELF in the future.

## Features
- **Learning-Oriented**: Designed as a learning tool for understanding executable structures.
- **Cross-Platform**: Minimal dependencies, no reliance on platform-specific headers.

## TODO
- [ ] Implement UI
- [ ] Allow modifications
- [ ] ELF support

## Building
To build the project, ensure you have C++17 or higher and ImGui installed. Use CMake for building:

```bash
git clone https://github.com/KL0-6/BinaryView.git
cd BinaryView
mkdir build && cd build
cmake ..
make
```
