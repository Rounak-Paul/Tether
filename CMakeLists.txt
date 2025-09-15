cmake_minimum_required(VERSION 3.15)

# Root project
project(TetherLanguage 
    VERSION 0.1.0
    DESCRIPTION "Tether Programming Language"
    LANGUAGES C
)

# C standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Platform detection and flags
if(WIN32)
    add_definitions(-DTETHER_PLATFORM_WINDOWS)
    message(STATUS "Platform: Windows")
elseif(APPLE)
    add_definitions(-DTETHER_PLATFORM_MACOS) 
    message(STATUS "Platform: macOS")
elseif(UNIX)
    add_definitions(-DTETHER_PLATFORM_LINUX)
    message(STATUS "Platform: Linux")
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

# Debug flag
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DTETHER_DEBUG)
    message(STATUS "Debug mode enabled")
endif()

# Build type default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Compiler flags
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
    endif()
elseif(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Od /Zi")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /O2")
    endif()
endif()

# Threading support
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

# Set output directories to project root bin folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)

# Add subdirectories for separate projects
add_subdirectory(Tether)
add_subdirectory(TetherNode)

# Summary
message(STATUS "")
message(STATUS "Tether Language Build Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C Standard: C${CMAKE_C_STANDARD}")
message(STATUS "")
message(STATUS "Projects:")
message(STATUS "  Tether Runtime: YES")
message(STATUS "  Tether Node: YES")
message(STATUS "")