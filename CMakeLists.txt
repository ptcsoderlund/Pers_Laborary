cmake_minimum_required(VERSION 3.23)

project(pl
  VERSION 0.1.0
  DESCRIPTION "PL — a productivity framework for C (Per Libraratory)"
  LANGUAGES C)

# Whether `pl` is built STATIC or SHARED is left entirely to the consumer via
# the standard BUILD_SHARED_LIBS switch (defaults to STATIC). We never force a
# library type here.

# Export compile_commands.json for clangd. Only meaningful with generators that
# support it (Ninja, Makefiles); harmless otherwise.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(pl)

# Convenience for clangd: mirror the generated compile_commands.json next to the
# sources so the language server finds it by walking up from any file in pl/.
# Only the build-tree copy is authoritative; this one is gitignored.
if(EXISTS "${CMAKE_BINARY_DIR}/compile_commands.json" OR CMAKE_EXPORT_COMPILE_COMMANDS)
  add_custom_target(copy_compile_commands ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${CMAKE_BINARY_DIR}/compile_commands.json"
      "${CMAKE_SOURCE_DIR}/compile_commands.json"
    COMMENT "Mirroring compile_commands.json to the source tree for clangd")
endif()
