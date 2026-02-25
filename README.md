# InterpKit v2.0.0

`InterpKit` is a fast, lightweight CLI (Command Line Interface) helper tool designed to speed up and simplify data lookup from engineering tables, especially the `d/L` function table for increments of `d/L`0.

![Demo InterpKit CLI](assets/interpkit.png)

This program was created to overcome the slow and error-prone manual process of finding upper and lower boundary values from tables, which are then used for linear interpolation calculations.

As of **v2.0.0**, InterpKit has been completely rewritten in **C++** (previously Python), making it significantly faster and available as a truly standalone, cross-platform executable with no runtime dependencies.

-----

## Main Features

* **Modern CLI Interface:** A clean, colorful, and highly responsive interactive terminal interface powered by FTXUI.
* **Fast Search:** Instantly finds the upper and lower boundary data rows based on your input value.
* **Interpolation Calculator:** Automatically performs linear interpolation once the boundary data is found.
* **Standalone Executable:** All required table data (`data_tabel.xlsx`) is fully embedded directly into the binary. No external files needed.

## Installation & Usage

InterpKit v2.0.0 provides pre-compiled standalone binaries for both Windows and Linux. 

1. Go to the [Releases](../../releases/latest) page on GitHub.
2. Download the appropriate executable for your operating system:
   - **Windows:** Download `interpkit.exe`
   - **Linux:** Download `interpkit`
3. Just double click or run the executable directly from your terminal! No Python, libraries, or external files are required.

## Building from Source

If you want to compile InterpKit yourself, you will need `cmake` and a C++17 compiler (e.g., GCC, Clang, MSVC).

```bash
git clone https://github.com/lainx86/InterpKit.git
cd InterpKit
./build.sh
```
The executable will be generated in the `build/` (Linux) or `build-win/` (Windows) directory.
