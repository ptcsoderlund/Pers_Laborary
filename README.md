# Pers_Laborary - a productivity framework for C

This is my laboratory for experimenting and learning C.
PL is a productivity library. Like a standard library.
Hopefully I can make C programming even more fun, stable and predictable.

Currently implementing basic stuff like:
- rust-like types in pl_types.h
- arena allocator,  
- custom malloc/free with statistics for tracking memory usage.
- dynamic array
- colored printing with formattable string inputs.
More to come.

It is a a normal CMake library; Consumers choice to link statically or dynamically.
C standard: **C23**.

## Build

The project uses CMake. The library target is `pl` (alias `pl::pl`). The dev
build output goes to `bin/debug/`.

build.bat on Windows
./build on linux

Visual studio dont generate compile_commands.json.
Which I need for clangd lsp.

# Comments
No roadmap, no vision, no plan.  
Just me learning C and having fun.  
