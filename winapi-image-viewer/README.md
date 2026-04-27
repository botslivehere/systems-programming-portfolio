# WinAPI Image Viewer

## Overview
A C++ application utilizing Windows API to parse, render, and manipulate a proprietary binary image format. The project validates proficiency with WinAPI GDI, struct packing, bitwise data extraction, and message loop handling.

## Technical Scope
- **File Parsing**: Reads binary headers and paletted pixel data using strict aligned structures (`#pragma pack(push, 1)`). Unpacks 5-bit compressed color indices via bitwise shifting.
- **Rendering System**: Employs `CreateCompatibleBitmap`, `StretchBlt`, and native Device Contexts (HDC) to project memory buffers onto the window.
- **Interactivity**: Captures and routes window messages (`WM_PAINT`, `WM_COMMAND`) to supply scaling, viewport panning, and coordinate grid overlays.