cmake_minimum_required(VERSION 3.20)

project(prismshell
  VERSION 0.1.0
  DESCRIPTION "PrismBASIC interactive shell and interpreter"
  LANGUAGES CXX
)

# ---- Options ---------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(USE_READLINE          "Use GNU Readline for input" ON)
option(BUILD_SHARED_LIBS     "Build prismshell_core as a shared library" OFF)
option(ENABLE_WARNINGS       "Enable compiler warnings" ON)
option(WARNINGS_AS_ERRORS    "Treat warnings as errors" OFF)
option(ENABLE_LTO            "Enable Link Time Optimization (IPO)" OFF)
option(INSTALL_DOCS          "Install docs and sample mods" ON)

include(GNUInstallDirs)

# ---- Library ---------------------------------------------------------------
add_library(prismshell_core
  src/lexer.cpp
  src/parser.cpp
  src/runtime.cpp
  src/interpreter.cpp
  src/utils.cpp
)

target_include_directories(prismshell_core
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

if(ENABLE_WARNINGS)
  if(MSVC)
    target_compile_options(prismshell_core PRIVATE /W4)
    if(WARNINGS_AS_ERRORS)
      target_compile_options(prismshell_core PRIVATE /WX)
    endif()
  else()
    target_compile_options(prismshell_core PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wno-sign-conversion)
    if(WARNINGS_AS_ERRORS)
      target_compile_options(prismshell_core PRIVATE -Werror)
    endif()
  endif()
endif()

# Optional IPO/LTO
if(ENABLE_LTO)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT _ipo_ok OUTPUT _ipo_msg)
  if(_ipo_ok)
    set_property(TARGET prismshell_core PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
  else()
    message(WARNING "IPO/LTO not supported: ${_ipo_msg}")
  endif()
endif()

# ---- Optional: Readline ----------------------------------------------------
if(USE_READLINE)
  find_path(READLINE_INCLUDE_DIR NAMES readline/readline.h)
  find_library(READLINE_LIBRARY NAMES readline)

  if(READLINE_INCLUDE_DIR AND READLINE_LIBRARY)
    add_library(Readline::Readline UNKNOWN IMPORTED)
    set_target_properties(Readline::Readline PROPERTIES
      IMPORTED_LOCATION             "${READLINE_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${READLINE_INCLUDE_DIR}"
    )
    target_compile_definitions(prismshell_core PUBLIC USE_READLINE)
    target_link_libraries(prismshell_core PUBLIC Readline::Readline)
    message(STATUS "Readline: ${READLINE_LIBRARY}")
  else()
    message(WARNING "USE_READLINE=ON but readline not found; building without it.")
  endif()
endif()

# ---- Executable ------------------------------------------------------------
add_executable(prismshell src/main.cpp)
target_link_libraries(prismshell PRIVATE prismshell_core)

# ---- Install ---------------------------------------------------------------
install(TARGETS prismshell RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Man pages (drop your files under man/man1 and man/man7 in the repo)
if(EXISTS "${PROJECT_SOURCE_DIR}/man/man1/prismshell.1")
  install(FILES
    ${PROJECT_SOURCE_DIR}/man/man1/prismshell.1
    DESTINATION ${CMAKE_INSTALL_MANDIR}/man1
  )
endif()

foreach(_page prismshell-language.7 prismshell-mods.7 prismshell-prompt.7)
  if(EXISTS "${PROJECT_SOURCE_DIR}/man/man7/${_page}")
    install(FILES "${PROJECT_SOURCE_DIR}/man/man7/${_page}"
            DESTINATION ${CMAKE_INSTALL_MANDIR}/man7)
  endif()
endforeach()

# Docs & sample mods (optional)
if(INSTALL_DOCS)
  if(EXISTS "${PROJECT_SOURCE_DIR}/docs")
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/docs/
      DESTINATION ${CMAKE_INSTALL_DOCDIR}
      FILES_MATCHING PATTERN "*.md")
  endif()

  if(EXISTS "${PROJECT_SOURCE_DIR}/mods")
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/mods/
      DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/prismshell/mods
      FILES_MATCHING PATTERN "*.bas")
  endif()
endif()
