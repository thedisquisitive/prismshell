# PrismBASIC Shell (C++ MVP)

A retro-style BASIC shell that doubles as your terminal. Type line-numbered BASIC like it’s 1983, or run modern commands via `/bin/sh`. Extend the shell using **mods written in PrismBASIC**. Make the prompt yours with a template or a `prompt` mod.

> Status: MVP. Small, clean core; designed to extend.

## Quick links

- [Installation](docs/Installation.md)
- [Getting Started](docs/Getting-Started.md)
- [Language Reference (MVP)](docs/Language.md)
- [Mods (PrismBASIC plugins)](docs/Mods.md)
- [Prompt Customization](docs/Prompt.md)
- [Troubleshooting](docs/Troubleshooting.md)
- [Architecture Overview](docs/Architecture.md)
- [Roadmap](docs/Roadmap.md)

## Quickstart

```bash
# build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j

# run
./prismshell           # interactive
./prismshell demo.bas  # run a program
