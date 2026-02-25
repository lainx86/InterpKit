# embed_resource.cmake
# Converts a binary file to a C++ header with a byte array
#
# Usage: cmake -DINPUT_FILE=... -DOUTPUT_FILE=... -P embed_resource.cmake

if(NOT DEFINED INPUT_FILE OR NOT DEFINED OUTPUT_FILE)
    message(FATAL_ERROR "INPUT_FILE and OUTPUT_FILE must be defined")
endif()

# Read binary file as hex
file(READ "${INPUT_FILE}" HEX_CONTENT HEX)
string(LENGTH "${HEX_CONTENT}" HEX_LENGTH)
math(EXPR BYTE_COUNT "${HEX_LENGTH} / 2")

# Convert hex pairs to 0xNN format with commas
string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," HEX_CONTENT "${HEX_CONTENT}")
# Add line breaks every 16 bytes (16 * 5 chars = 80 chars per line)
string(REGEX REPLACE "([^\n]{80})" "\\1\n    " HEX_CONTENT "${HEX_CONTENT}")

# Write output header
file(WRITE "${OUTPUT_FILE}"
"// Auto-generated file - DO NOT EDIT
// Embedded resource from: ${INPUT_FILE}
#pragma once

#include <cstddef>
#include <cstdint>

namespace embedded {

inline const uint8_t XLSX_DATA[] = {
    ${HEX_CONTENT}
};

inline constexpr size_t XLSX_DATA_SIZE = ${BYTE_COUNT};

} // namespace embedded
")
