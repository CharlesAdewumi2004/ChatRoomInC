# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(ChatProgram VERSION 1.0)

# Specify the C standard
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED True)

# Add the source files
set(SOURCES
    src/main.c
    src/server.c
    src/client.c
)

# Include header files
include_directories(src)

# Create the executable
add_executable(ChatProgram ${SOURCES})

target_link_libraries(ChatProgram PRIVATE pthread)

