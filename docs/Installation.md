# Installation

## Requirements
- CMake ≥ 3.15
- A C++17 (or newer) compiler (gcc/clang)
- POSIX system recommended (Linux/macOS/WSL). Windows builds stub a few POSIX calls.
- Optional: GNU Readline (`libreadline-dev`) if building with line-editing support.

## Build

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j
```

Run:

```bash
./prismshell
# or run a program
./prismshell path/to/script.bas
```

## Install (optional)

```bash
cmake --install build --prefix ~/.local
# handy alias
ln -sf ~/.local/bin/prismshell ~/.local/bin/pbsh
```

> If `cmake --install` errors with “Not a file … cmake_install.cmake”, you skipped configure. Run:
> `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release` first.
