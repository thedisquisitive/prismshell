# Installation

## Requirements

### Build Tools
- **CMake** ≥ 3.20
- **C++17 (or newer) compiler**:
  - GCC ≥ 7.0
  - Clang ≥ 5.0
  - MSVC ≥ 2017 (Visual Studio 15.0)
- **Make** or **Ninja** (build system backend)

### System Requirements
- **POSIX-compliant system** recommended (Linux, macOS, BSD, WSL)
- Windows native builds supported (some features stubbed, see notes below)

### Optional Dependencies
- **GNU Readline** (`libreadline-dev` or `readline-devel`)
  - Provides line editing, history, and completion
  - Highly recommended for interactive use
  - Build with `-DUSE_READLINE=ON` (default)

### Runtime Dependencies
- **POSIX shell** (`/bin/sh`) for shell passthrough feature
  - Pre-installed on Linux, macOS, BSD
  - Available via WSL on Windows
  - Limited functionality on native Windows builds

## Platform-Specific Setup

### Ubuntu/Debian
```bash
# Install build tools and dependencies
sudo apt-get update
sudo apt-get install -y \
  build-essential \
  cmake \
  libreadline-dev \
  git
```

### Fedora/RHEL/CentOS
```bash
# Install build tools and dependencies
sudo dnf install -y \
  gcc-c++ \
  cmake \
  readline-devel \
  make \
  git
```

### Arch Linux
```bash
# Install build tools and dependencies
sudo pacman -S --needed \
  base-devel \
  cmake \
  readline \
  git
```

### macOS
```bash
# Install Xcode Command Line Tools (includes clang)
xcode-select --install

# Install Homebrew (if not already installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake readline

# Note: macOS includes /bin/sh by default
```

### Windows (Native)
```powershell
# Install Visual Studio 2019 or later with C++ support
# Download from: https://visualstudio.microsoft.com/downloads/

# Install CMake
# Download from: https://cmake.org/download/

# Note: Native Windows builds stub some POSIX features
# For full functionality, use WSL (see below)
```

### Windows (WSL - Recommended)
```bash
# From Windows, install WSL2 with Ubuntu
wsl --install

# Inside WSL, follow Ubuntu instructions above
sudo apt-get update
sudo apt-get install -y build-essential cmake libreadline-dev git
```

## Build from Source

### Quick Build (Unix-like systems)

The project includes a convenience build script:

```bash
git clone https://github.com/yourusername/prismshell.git
cd prismshell
./build.sh
```

This script performs:
1. Configuration: `cmake -S . -B build`
2. Compilation: `cmake --build build -j`
3. Installation: `sudo cmake --install build --prefix /usr/local`

### Manual Build Steps

#### 1. Configure
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
```

#### 2. Compile
```bash
cmake --build . -j$(nproc)  # Linux
cmake --build . -j$(sysctl -n hw.ncpu)  # macOS
cmake --build . -j  # Auto-detect cores
```

#### 3. Test (optional)
```bash
./prismshell ../test.bas
```

#### 4. Install (optional)
```bash
# System-wide installation
sudo cmake --install . --prefix /usr/local

# User-local installation
cmake --install . --prefix ~/.local

# Add to PATH if using user-local
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc
```

## Build Options

Configure the build with CMake options:

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DUSE_READLINE=ON \
  -DBUILD_SHARED_LIBS=OFF \
  -DENABLE_WARNINGS=ON \
  -DWARNINGS_AS_ERRORS=OFF \
  -DENABLE_LTO=ON \
  -DINSTALL_DOCS=ON
```

### Available Options

| Option | Default | Description |
|--------|---------|-------------|
| `CMAKE_BUILD_TYPE` | `Release` | Build type: `Debug`, `Release`, `RelWithDebInfo`, `MinSizeRel` |
| `USE_READLINE` | `ON` | Enable GNU Readline for line editing |
| `BUILD_SHARED_LIBS` | `OFF` | Build `prismshell_core` as shared library |
| `ENABLE_WARNINGS` | `ON` | Enable compiler warnings |
| `WARNINGS_AS_ERRORS` | `OFF` | Treat warnings as errors |
| `ENABLE_LTO` | `OFF` | Enable Link Time Optimization (IPO) |
| `INSTALL_DOCS` | `ON` | Install documentation and sample mods |

### Build Type Details

- **Debug**: No optimization, full debug symbols, assertions enabled
- **Release**: Full optimization (-O3), no debug symbols (recommended for production)
- **RelWithDebInfo**: Optimized with debug symbols (good for profiling)
- **MinSizeRel**: Optimize for size

## Installation Paths

Default installation paths with `--prefix /usr/local`:

| Component | Path |
|-----------|------|
| Executable | `/usr/local/bin/prismshell` |
| Man pages (section 1) | `/usr/local/share/man/man1/` |
| Man pages (section 7) | `/usr/local/share/man/man7/` |
| Documentation | `/usr/local/share/doc/prismshell/` |
| Sample mods | `/usr/local/share/prismshell/mods/` |

User-local installation (`--prefix ~/.local`):

| Component | Path |
|-----------|------|
| Executable | `~/.local/bin/prismshell` |
| Man pages | `~/.local/share/man/man{1,7}/` |
| Documentation | `~/.local/share/doc/prismshell/` |
| Sample mods | `~/.local/share/prismshell/mods/` |

## Post-Installation

### Verify Installation

```bash
# Check version and help
prismshell --help  # (if implemented)
prismshell

# In the shell, type:
HELP
BYE

# Run test suite
prismshell /usr/local/share/doc/prismshell/test.bas
```

### Set Up Mods Directory (Optional)

```bash
# Create user mods directory
mkdir -p ~/.config/prismshell/mods

# Copy sample mods
cp /usr/local/share/prismshell/mods/*.bas ~/.config/prismshell/mods/

# Or symlink
ln -s /usr/local/share/prismshell/mods ~/.config/prismshell/mods
```

### Configure Autostart (Optional)

Create `~/.prismrc` to control startup behavior:

```bash
cat > ~/.prismrc << 'EOF'
# Only autostart selected mods
autoload_all = 0
autostart = prompt.bas

# Or autostart all discovered mods
# autoload_all = 1
EOF
```

### Add Convenient Alias (Optional)

```bash
# Add to ~/.bashrc, ~/.zshrc, etc.
echo 'alias pbsh=prismshell' >> ~/.bashrc
source ~/.bashrc
```

## Troubleshooting Installation

### "CMake version too old"
```bash
# Ubuntu/Debian: Add Kitware's apt repository for latest CMake
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | \
  gpg --dearmor - | sudo tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null
sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ focal main'
sudo apt-get update
sudo apt-get install cmake

# Or use snap
sudo snap install cmake --classic

# Or build from source
# https://cmake.org/download/
```

### "Readline not found"
```bash
# Debian/Ubuntu
sudo apt-get install libreadline-dev

# Fedora/RHEL
sudo dnf install readline-devel

# macOS
brew install readline

# Or build without readline
cmake -S . -B build -DUSE_READLINE=OFF
```

### "Not a file … cmake_install.cmake"
This means you skipped the configure step. Run:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
cmake --install build --prefix ~/.local
```

### Permission denied during installation
```bash
# Use sudo for system-wide
sudo cmake --install build --prefix /usr/local

# Or install to user directory (no sudo needed)
cmake --install build --prefix ~/.local
```

### Man pages not found after installation
```bash
# Ensure MANPATH includes install location
echo 'export MANPATH="$HOME/.local/share/man:$MANPATH"' >> ~/.bashrc
source ~/.bashrc

# Or for system install, update mandb
sudo mandb
```

## Building for Development

For development builds with debugging enabled:

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DENABLE_WARNINGS=ON \
  -DWARNINGS_AS_ERRORS=OFF

cmake --build build -j

# Run with debugger
gdb build/prismshell
lldb build/prismshell
```

The `compile_commands.json` file is useful for IDEs and language servers (clangd, etc.).

## Cross-Compilation

Example for ARM64 target:

```bash
cmake -S . -B build-arm64 \
  -DCMAKE_TOOLCHAIN_FILE=path/to/arm64-toolchain.cmake \
  -DCMAKE_BUILD_TYPE=Release

cmake --build build-arm64 -j
```

## Static Linking (for portable binaries)

```bash
cmake -S . -B build-static \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_EXE_LINKER_FLAGS="-static" \
  -DUSE_READLINE=OFF

cmake --build build-static -j
```

Note: Static linking may not work on all platforms (especially macOS) and disables Readline.

## Uninstallation

```bash
# From build directory
sudo cmake --build build --target uninstall  # if supported

# Or manually
sudo rm /usr/local/bin/prismshell
sudo rm /usr/local/share/man/man1/prismshell.1
sudo rm /usr/local/share/man/man7/prismshell-*.7
sudo rm -rf /usr/local/share/doc/prismshell
sudo rm -rf /usr/local/share/prismshell
```

## Next Steps

After installation:
1. Read [Getting Started](Getting-Started.md)
2. Review [Language Reference](Language.md)
3. Explore [Mods documentation](Mods.md)
4. Run the test suite: `prismshell test.bas`