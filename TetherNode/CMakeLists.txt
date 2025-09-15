# Tether Node Project
project(TetherNode LANGUAGES C)

# Collect all source files from src and its subdirectories
file(GLOB_RECURSE NODE_SOURCES
    "src/*.c"
)

# Collect all header files
file(GLOB_RECURSE NODE_HEADERS
    "src/*.h"
)

# Create the tether-node executable
add_executable(tether-node ${NODE_SOURCES})

# Include directories
target_include_directories(tether-node PRIVATE src)

# Link threading library
target_link_libraries(tether-node PRIVATE Threads::Threads)

# Platform-specific libraries (node needs networking)
if(TETHER_PLATFORM_LINUX)
    target_link_libraries(tether-node PRIVATE m dl)
elseif(TETHER_PLATFORM_WINDOWS)
    target_link_libraries(tether-node PRIVATE ws2_32)
elseif(TETHER_PLATFORM_MACOS)
    target_link_libraries(tether-node PRIVATE m)
endif()

# Set output directory
set_target_properties(tether-node PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

message(STATUS "Tether node configured")
if(NODE_SOURCES)
    list(LENGTH NODE_SOURCES SOURCE_COUNT)
    message(STATUS "  Source files: ${SOURCE_COUNT}")
endif()