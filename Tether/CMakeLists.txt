# Tether Runtime Project
project(Tether LANGUAGES C)

# Collect all source files from src and its subdirectories
file(GLOB_RECURSE TETHER_SOURCES
    "src/*.c"
)

# Collect all header files
file(GLOB_RECURSE TETHER_HEADERS
    "src/*.h"
)

# Create the tether executable
add_executable(tether ${TETHER_SOURCES})

# Include directories
target_include_directories(tether PRIVATE src)

# Link threading library
target_link_libraries(tether PRIVATE Threads::Threads)

# Platform-specific libraries
if(TETHER_PLATFORM_LINUX)
    target_link_libraries(tether PRIVATE m dl)
elseif(TETHER_PLATFORM_WINDOWS)
    target_link_libraries(tether PRIVATE ws2_32)
elseif(TETHER_PLATFORM_MACOS)
    target_link_libraries(tether PRIVATE m)
endif()

# Set output directory
set_target_properties(tether PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

message(STATUS "Tether runtime configured")
message(STATUS "  Sources found: ${CMAKE_CURRENT_LIST_LENGTH}")
if(TETHER_SOURCES)
    list(LENGTH TETHER_SOURCES SOURCE_COUNT)
    message(STATUS "  Source files: ${SOURCE_COUNT}")
endif()