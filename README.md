# Low-Level Systems and Architecture Portfolio

This repository contains a structured collection of systems programming projects. The primary focus is on cross-language integration (C/C++ and Assembly), direct memory management, hardware interrupt handling, and operating system API utilization (WinAPI).

## Competency Matrix

- **Languages:** C, C++, Assembly (x86 architecture, TASM), Java
- **Systems & APIs:** Windows API (GDI), DOS Real-Mode execution, Java standard libraries (Sockets, HTTP)
- **Core Engineering Concepts:** Bare-metal hardware interaction, Interrupt Vector Table (IVT) hooking, struct-packing for binary serialization, memory segmentation, modular architecture, socket-based networking.

## Project Index

| Project Directory | Synopsis | Primary Technologies |
| :--- | :--- | :--- |
| [`winapi-image-viewer`](./winapi-image-viewer/) | Native application for decoding and rendering a custom tightly-packed binary image format. Implements GDI rendering, interactive UI controls, and algorithmic scaling. | C++, WinAPI, Bitwise Operations |
| [`c-library-client`](./c-library-client/) | Demonstration of interface segregation and library design. Splits logic into a robust linkable component and a consuming client executable. | C/C++, Architecture |
| [`asm-real-interrupts`](./asm-real-interrupts/) | Mixed-language module (C and Assembly) implementing direct hardware and software interrupt handling in a DOS real-mode environment. | C, Assembly |
| [`asm-interrupt-handling`](./asm-interrupt-handling/) | Assembly implementation of CPU trap routines and interrupt service routines (ISR). Highlights register state preservation. | Assembly (x86) |
| [`asm-iop-processor`](./asm-iop-processor/) | Simulation of input/output processor mechanisms. Focuses on offloading I/O bottlenecks. | Assembly |
| [`java-base-networking`](./java-base-networking/) | Foundational networking module demonstrating raw socket connections, HTTP protocol implementation, and basic Servlet architecture. | Java, Sockets, HTTP, Maven |
| [`tasm-solution`](./tasm-solution/) | Legacy 16-bit application developed with Turbo Assembler. Validates understanding of historical architectures and segment registers. | TASM |
| [`turbo-cpp-graphics`](./turbo-cpp-graphics/) | Application utilizing the Borland Graphics Interface to demonstrate direct pixel manipulation and primitive rasterization. | C++, BGI |

## Implementation Standards

- **Minimal Dependencies:** Code relies strictly on native OS libraries and standard runtime libraries.
- **Resource Management:** Strict allocation and tracking of memory blocks and graphical device contexts.
- **Optimization:** Use of bit-level unpacking and packed structs for memory-efficient binary parsing.