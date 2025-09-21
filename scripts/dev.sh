#!/usr/bin/env bash
# Simple helper for common CMake tasks.
set -euo pipefail

# Defaults
BUILD_TYPE="${BUILD_TYPE:-Release}"
BUILD_DIR="${BUILD_DIR:-build/${BUILD_TYPE}}"
GEN="${GEN:-Ninja}"   # change to "Unix Makefiles" if you prefer

usage() {
  cat <<'EOF'
Usage: scripts/dev.sh <cmd> [args]

Commands:
  cfg        Configure (uses $BUILD_TYPE, $BUILD_DIR, $GEN)
  dbg        Configure Debug build
  rel        Configure Release build
  build|-b   Build
  run|-r     Run prismshell
  test|-t    CTest (if tests are added)
  install|-i Install to --prefix (default /usr/local, override PREFIX)
  clean      Remove build dir
  shell      Open subshell in build dir

Env:
  BUILD_TYPE=Debug|Release (default Release)
  BUILD_DIR=build/<type>   (default build/$BUILD_TYPE)
  GEN=Ninja|Unix Makefiles (default Ninja)
  PREFIX=/usr/local        (for install)
  USE_READLINE=ON|OFF      (propagated to CMake)
EOF
}

cmd=${1:-help}
shift || true

case "$cmd" in
  cfg)
    cmake -S . -B "$BUILD_DIR" -G "$GEN" \
      -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
      -DUSE_READLINE="${USE_READLINE:-ON}" \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    ;;

  dbg) BUILD_TYPE=Debug "$0" cfg ;;

  rel) BUILD_TYPE=Release "$0" cfg ;;

  build|-b)
    cmake --build "$BUILD_DIR" -j"${JOBS:-}"
    ;;

  run|-r)
    cmake --build "$BUILD_DIR" -j"${JOBS:-}"
    exec "$BUILD_DIR/prismshell" "$@"
    ;;

  test|-t)
    ctest --test-dir "$BUILD_DIR" --output-on-failure
    ;;

  install|-i)
    cmake --build "$BUILD_DIR" -j"${JOBS:-}"
    cmake --install "$BUILD_DIR" --prefix "${PREFIX:-/usr/local}"
    ;;

  clean)
    rm -rf "$BUILD_DIR"
    ;;

  shell)
    cd "$BUILD_DIR"
    exec ${SHELL:-bash}
    ;;

  help|--help|-h)
    usage
    ;;

  *)
    echo "Unknown command: $cmd"
    usage
    exit 1
    ;;
esac
