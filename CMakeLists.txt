cmake_minimum_required(VERSION 3.14)
project(InterpKit VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ──────────────────────────────────────────────
# Dependencies via FetchContent
# ──────────────────────────────────────────────
include(FetchContent)

FetchContent_Declare(
    OpenXLSX
    GIT_REPOSITORY https://github.com/troldal/OpenXLSX.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE
)

FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
    GIT_TAG        v5.0.0
    GIT_SHALLOW    TRUE
)

# Disable unnecessary targets for dependencies to speed up build and silence warnings
set(FTXUI_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(FTXUI_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(FTXUI_BUILD_TESTS OFF CACHE BOOL "" FORCE)

set(OPENXLSX_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(OPENXLSX_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(OpenXLSX ftxui)

# ──────────────────────────────────────────────
# Executable
# ──────────────────────────────────────────────
add_executable(interpkit
    src/main.cpp
    src/logic.cpp
    src/ui.cpp
)

target_include_directories(interpkit PRIVATE src)

target_link_libraries(interpkit PRIVATE
    OpenXLSX::OpenXLSX
    ftxui::screen
    ftxui::dom
    ftxui::component
)

# ──────────────────────────────────────────────
# Embed data_tabel.xlsx into the binary
# ──────────────────────────────────────────────
set(EMBED_INPUT  "${CMAKE_SOURCE_DIR}/data_tabel.xlsx")
set(EMBED_OUTPUT "${CMAKE_BINARY_DIR}/generated/embedded_data.h")

add_custom_command(
    OUTPUT  ${EMBED_OUTPUT}
    COMMAND ${CMAKE_COMMAND}
            -DINPUT_FILE=${EMBED_INPUT}
            -DOUTPUT_FILE=${EMBED_OUTPUT}
            -P ${CMAKE_SOURCE_DIR}/cmake/embed_resource.cmake
    DEPENDS ${EMBED_INPUT}
            ${CMAKE_SOURCE_DIR}/cmake/embed_resource.cmake
    COMMENT "Embedding data_tabel.xlsx into binary..."
)

add_custom_target(embed_data DEPENDS ${EMBED_OUTPUT})
add_dependencies(interpkit embed_data)

target_include_directories(interpkit PRIVATE ${CMAKE_BINARY_DIR}/generated)
