# Tether Runtime Project
project(Tether LANGUAGES C)

# Collect all source files from src and its subdirectories
file(GLOB_RECURSE TETHER_SOURCES
    "src/*.c"
)

# Build a list of sources without the application entrypoint (src/main.c)
set(TETHER_APP_SOURCES "")
foreach(s IN LISTS TETHER_SOURCES)
    if(NOT s MATCHES ".*/src/main.c$")
        list(APPEND TETHER_APP_SOURCES ${s})
    endif()
endforeach()

# Collect all header files
file(GLOB_RECURSE TETHER_HEADERS
    "src/*.h"
)

# Create the tether executable
add_executable(tether ${TETHER_APP_SOURCES})

# Include directories
target_include_directories(tether PRIVATE src)

# Unit tests (small, local)
file(GLOB_RECURSE TETHER_TEST_SOURCES "tests/unit/*.c")
if(TETHER_TEST_SOURCES)
    # Link tests with library sources (excluding the runtime main)
    add_executable(tether_tests ${TETHER_TEST_SOURCES} ${TETHER_APP_SOURCES})
    target_include_directories(tether_tests PRIVATE src)
    target_link_libraries(tether_tests PRIVATE Threads::Threads)
    # Put tests in build/bin as well
    set_target_properties(tether_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    add_custom_target(test-core
        COMMAND $<TARGET_FILE:tether_tests>
        DEPENDS tether_tests
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()

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