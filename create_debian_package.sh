#!/usr/bin/env bash
# create-debian-package.sh - Build a .deb package for PrismShell
# Fixed version - compliant with Debian packaging standards

set -e

PROJECT_NAME="prismshell"
VERSION="0.1.0"
ARCH="amd64"  # or arm64, armhf, etc.
MAINTAINER="Zach Hogarth <zach@thedisquisitive.com>"
DESCRIPTION="PrismBASIC interactive shell and interpreter"

# Create packaging directory
PKG_DIR="${PROJECT_NAME}_${VERSION}_${ARCH}"

# Clean up any previous build (may need sudo if owned by root)
if [ -d "$PKG_DIR" ]; then
    if [ -w "$PKG_DIR" ]; then
        rm -rf "$PKG_DIR"
    else
        echo "Cleaning up previous build (requires sudo)..."
        sudo rm -rf "$PKG_DIR"
    fi
fi

mkdir -p "$PKG_DIR"

# Create directory structure (use /usr instead of /usr/local for proper Debian packages)
mkdir -p "$PKG_DIR/DEBIAN"
mkdir -p "$PKG_DIR/usr/bin"
mkdir -p "$PKG_DIR/usr/share/man/man1"
mkdir -p "$PKG_DIR/usr/share/man/man7"
mkdir -p "$PKG_DIR/usr/share/doc/$PROJECT_NAME"
mkdir -p "$PKG_DIR/usr/share/$PROJECT_NAME/mods"
mkdir -p "$PKG_DIR/usr/share/$PROJECT_NAME/examples"

# Build the project with /usr prefix
echo "Building $PROJECT_NAME..."
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DUSE_READLINE=ON -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build -j

# Copy binary and strip it (removes debug symbols)
echo "Copying files..."
cp build/prismshell "$PKG_DIR/usr/bin/"
strip "$PKG_DIR/usr/bin/prismshell"
chmod 755 "$PKG_DIR/usr/bin/prismshell"

# Copy man pages with reproducible timestamps (-n flag)
cp man/man1/prismshell.1 "$PKG_DIR/usr/share/man/man1/"
cp man/man7/*.7 "$PKG_DIR/usr/share/man/man7/"
gzip -9fn "$PKG_DIR/usr/share/man/man1/"*.1
gzip -9fn "$PKG_DIR/usr/share/man/man7/"*.7

# Copy documentation
cp docs/*.md "$PKG_DIR/usr/share/doc/$PROJECT_NAME/"
cp readme.md "$PKG_DIR/usr/share/doc/$PROJECT_NAME/"

# Copy test.bas to examples directory (not doc) to avoid shebang warning
cp test.bas "$PKG_DIR/usr/share/$PROJECT_NAME/examples/"

# Copy sample mods
cp mods/*.bas "$PKG_DIR/usr/share/$PROJECT_NAME/mods/"

# Create control file
cat > "$PKG_DIR/DEBIAN/control" <<EOF
Package: $PROJECT_NAME
Version: $VERSION
Section: shells
Priority: optional
Architecture: $ARCH
Depends: libc6 (>= 2.27), libstdc++6 (>= 8), libreadline8 (>= 7.0)
Maintainer: $MAINTAINER
Homepage: https://github.com/thedisquisitive/prismshell
Description: $DESCRIPTION
 A retro-style BASIC shell featuring a line-numbered editor, direct execution
 mode, arrays, user-defined subroutines, and block control flow. Includes
 comprehensive filesystem operations, mod system for extensibility, and
 seamless shell passthrough for Unix command integration.
 .
 Phase 1 & 2 Complete - Full-featured classic BASIC interpreter:
  - Arrays: fixed (DIM arr[10]) and dynamic (DIM arr[])
  - User SUBs with parameters, recursion, and return values
  - Block IF/ELSEIF/ELSE/ENDIF
  - WHILE/WEND loops
  - FOR/NEXT loops with optional STEP (positive or negative)
  - DATA/READ/RESTORE for embedded data
  - READ into arrays and variables
  - 50+ builtin functions (filesystem, paths, env, RNG, time)
  - Mod system for writing shell extensions in BASIC
  - Customizable prompts
  - Shebang script support
EOF

# Create postinst script
cat > "$PKG_DIR/DEBIAN/postinst" <<'EOF'
#!/bin/sh
set -e

# Update man database
if command -v mandb >/dev/null 2>&1; then
    mandb -q
fi

echo "PrismShell installed successfully!"
echo "Run 'prismshell' to start, or 'man prismshell' for documentation."
echo "Example scripts: /usr/share/prismshell/examples/"

exit 0
EOF
chmod 755 "$PKG_DIR/DEBIAN/postinst"

# Create prerm script
cat > "$PKG_DIR/DEBIAN/prerm" <<'EOF'
#!/bin/sh
set -e

# Cleanup user mods directory if empty
if [ -d "$HOME/.config/prismshell/mods" ]; then
    rmdir "$HOME/.config/prismshell/mods" 2>/dev/null || true
    rmdir "$HOME/.config/prismshell" 2>/dev/null || true
fi

exit 0
EOF
chmod 755 "$PKG_DIR/DEBIAN/prerm"

# Create copyright file (Debian requires this)
cat > "$PKG_DIR/usr/share/doc/$PROJECT_NAME/copyright" <<'EOF'
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: prismshell
Source: https://github.com/thedisquisitive/prismshell

Files: *
Copyright: 2025 Zach Hogarth <zach@thedisquisitive.com>
License: MIT
 Permission is hereby granted, free of charge, to any person obtaining a
 copy of this software and associated documentation files (the "Software"),
 to deal in the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:
 .
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
 .
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.
EOF

# Create changelog (required for native Debian packages)
cat > "$PKG_DIR/usr/share/doc/$PROJECT_NAME/changelog" <<EOF
$PROJECT_NAME ($VERSION) unstable; urgency=low

  * Initial release
  * Phase 1 & 2 COMPLETE
  * Arrays with fixed and dynamic sizing (DIM name[size] and DIM name[])
  * User-defined SUBs with parameters, recursion, and return values
  * Block control flow: IF/ELSEIF/ELSE/ENDIF and WHILE/WEND
  * FOR/NEXT loops with optional STEP (including negative step)
  * DATA/READ/RESTORE for embedded data
  * READ into arrays (READ arr[index])
  * Comprehensive builtin functions (FS.*, Path.*, Env.*, RNG.*, Time.*)
  * Mod system for shell extensibility
  * Customizable prompts via templates or mods
  * Shell passthrough for seamless Unix integration
  * Shebang script support

 -- $MAINTAINER  $(date -R)
EOF
gzip -9fn "$PKG_DIR/usr/share/doc/$PROJECT_NAME/changelog"

# Calculate installed size BEFORE changing ownership
INSTALLED_SIZE=$(du -sk "$PKG_DIR" | cut -f1)
echo "Installed-Size: $INSTALLED_SIZE" >> "$PKG_DIR/DEBIAN/control"

# Set proper ownership (root:root) for package files
echo "Setting ownership..."
sudo chown -R root:root "$PKG_DIR"

# Build the package with proper ownership
echo "Building .deb package..."
sudo dpkg-deb --build --root-owner-group "$PKG_DIR"

# Restore ownership to current user
sudo chown "$USER:$USER" "${PKG_DIR}.deb"

# Verify the package
echo ""
echo "Verifying package..."
lintian "${PKG_DIR}.deb" || true

echo ""
echo "=========================================="
echo "Package created: ${PKG_DIR}.deb"
echo "=========================================="
echo ""
echo "Install with:"
echo "  sudo apt install ./${PKG_DIR}.deb"
echo ""
echo "Or manually:"
echo "  sudo dpkg -i ${PKG_DIR}.deb"
echo "  sudo apt-get install -f  # Fix dependencies if needed"
echo ""
echo "Test installation:"
echo "  prismshell /usr/share/prismshell/examples/test.bas"
echo ""