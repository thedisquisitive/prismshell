#!/usr/bin/env bash
# PrismShell release packer — creates a reproducible prismshell-<version>.tar.gz
# Usage:
#   scripts/make-dist.sh [--version <ver>] [--sign] [--outdir <dir>]
# Examples:
#   scripts/make-dist.sh --version v0.1.0-alpha.1
#   scripts/make-dist.sh --sign
set -euo pipefail

# --- defaults ---------------------------------------------------------------
OUTDIR="${PWD}/dist"
SIGN=0
VERSION=""

# --- args -------------------------------------------------------------------
while [[ $# -gt 0 ]]; do
  case "$1" in
    --version) VERSION="${2:-}"; shift 2;;
    --outdir)  OUTDIR="${2:-}"; shift 2;;
    --sign)    SIGN=1; shift;;
    -h|--help)
      sed -n '1,30p' "$0"; exit 0;;
    *) echo "Unknown arg: $1" >&2; exit 1;;
  esac
done

# --- sanity checks -----------------------------------------------------------
[[ -f CMakeLists.txt && -d src && -d include ]] || {
  echo "Run from repo root (need CMakeLists.txt, src/, include/)" >&2; exit 1;
}
mkdir -p "$OUTDIR"

# --- determine version -------------------------------------------------------
if [[ -z "$VERSION" ]]; then
  if git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
    if ver=$(git describe --tags --dirty --always 2>/dev/null); then
      VERSION="$ver"
    fi
  fi
fi
if [[ -z "$VERSION" ]]; then
  VERSION="0.1.0-alpha.$(date -u +%Y%m%d)"
fi
# For filenames, strip leading 'v'
FNVER="${VERSION#v}"
PKGNAME="prismshell-${FNVER}"

# --- temp staging ------------------------------------------------------------
TMPROOT="$(mktemp -d 2>/dev/null || mktemp -d -t prismshell-dist)"
trap 'rm -rf "$TMPROOT"' EXIT
STAGE="${TMPROOT}/${PKGNAME}"
mkdir -p "$STAGE"

# --- collect files -----------------------------------------------------------
# Prefer git index (clean set); fallback to curated copy if not a git tree.
if git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
  echo "Using git index for file list…"
  git ls-files -z | while IFS= read -r -d '' f; do
    dest="${STAGE}/$f"
    mkdir -p "$(dirname "$dest")"
    cp --parents -a "$f" "$STAGE" 2>/dev/null || cp -a "$f" "$dest"
  done
else
  echo "Not a git repo; copying common project files…"
  for p in \
    CMakeLists.txt CMakePresets.json cmake \
    src include mods man docs examples scripts \
    README* LICENSE* CHANGELOG* \
  ; do
    [[ -e "$p" ]] || continue
    cp -a "$p" "$STAGE/"
  done
fi

# Ensure a VERSION file exists in the tarball
printf '%s\n' "$VERSION" > "${STAGE}/VERSION"

# Optional: ensure man pages exist (warn only)
if [[ ! -d "${STAGE}/man" ]]; then
  echo "Warning: man/ directory not found; manpages won’t be in the tarball." >&2
fi

# --- reproducible tarball ----------------------------------------------------
EPOCH="${SOURCE_DATE_EPOCH:-$(date -u +%s)}"
pushd "$TMPROOT" >/dev/null

TARBALL="${OUTDIR}/${PKGNAME}.tar.gz"
echo "Packing ${TARBALL} …"
# Reproducible: sort names, zero out uid/gid, fix mtime, gzip -n (no timestamp)
tar --sort=name \
    --owner=0 --group=0 --numeric-owner \
    --mtime="@${EPOCH}" \
    -cf - "${PKGNAME}" | gzip -n > "${TARBALL}"

popd >/dev/null

# --- checksums & (optional) signature ---------------------------------------
( cd "$(dirname "$TARBALL")" && sha256sum "$(basename "$TARBALL")" > "${PKGNAME}.tar.gz.sha256" )
echo "SHA256: ${TARBALL}.sha256"

if (( SIGN )); then
  echo "Signing ${TARBALL} with GPG (detached, ASCII armor)…"
  gpg --armor --detach-sign --output "${TARBALL}.asc" "${TARBALL}"
  echo "Sig: ${TARBALL}.asc"
fi

echo "Done:"
echo "  ${TARBALL}"
echo "  ${TARBALL}.sha256"
(( SIGN )) && echo "  ${TARBALL}.asc"
