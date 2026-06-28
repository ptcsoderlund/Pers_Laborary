# The `pl` library target.
#
# Sources and public headers are globbed (CONFIGURE_DEPENDS makes CMake re-glob
# on build, so newly added files are picked up without a manual re-configure).
file(GLOB_RECURSE PL_SOURCES CONFIGURE_DEPENDS src/*.c)
list(FILTER PL_SOURCES EXCLUDE REGEX ".*/sandbox/.*")
file(GLOB_RECURSE PL_HEADERS CONFIGURE_DEPENDS include/*.h)

# No STATIC/SHARED keyword: the type follows BUILD_SHARED_LIBS, so the consumer
# decides static vs. dynamic linking.
add_library(pl ${PL_SOURCES})
add_library(pl::pl ALIAS pl)

# FILE_SET HEADERS wires up the include search path for both the build tree
# (BASE_DIRS) and the install tree, and installs the headers preserving the
# sys/ and sys/collections/ subdirectory structure — no manual
# target_include_directories needed.
target_sources(pl
  PUBLIC
    FILE_SET pl_headers TYPE HEADERS
      BASE_DIRS include
      FILES ${PL_HEADERS})

# C23, no compiler extensions; PUBLIC so consumers inherit the requirement.
target_compile_features(pl PUBLIC c_std_23)
set_target_properties(pl PROPERTIES
  C_STANDARD 23
  C_STANDARD_REQUIRED ON
  C_EXTENSIONS OFF
  # Auto-export all symbols when built as a DLL on Windows, so a SHARED build is
  # usable without annotating every public function with __declspec(dllexport).
  WINDOWS_EXPORT_ALL_SYMBOLS ON
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR})

# ---------------------------------------------------------------------------
# Install + find_package(pl) support
# ---------------------------------------------------------------------------
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS pl
  EXPORT plTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  FILE_SET pl_headers DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT plTargets
  FILE plTargets.cmake
  NAMESPACE pl::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pl)

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/plConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/plConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pl)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/plConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/plConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/plConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pl)
