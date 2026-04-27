# Real-Mode Interrupt Handlers

## Overview
Low-level implementation coupling C code and Assembly macros to address hardware and software interrupts within real-mode memory limits.

## Technical Scope
- Configures the Interrupt Vector Table (IVT) directly via memory pointing.
- Intercepts default routines, pushes CPU registers, and restores program execution streams.
- Facilitates the ABI (Application Binary Interface) mapping between C function calls and Assembly execution.