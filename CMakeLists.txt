cmake_minimum_required(VERSION 3.20)

project(moldcast
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# ---------------- library ----------------

add_library(moldcast
    src/header.cpp
    src/socket.cpp
    src/epollWrapper.cpp
)

target_include_directories(moldcast PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/moldcast
)

target_compile_options(moldcast PRIVATE
    -Wall -Wextra -Wpedantic
)

# ---------------- sanitizers ----------------

option(MOLDCAST_SANITIZERS "Enable ASan and UBSan" ON)

if(MOLDCAST_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(moldcast PUBLIC
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    )
    target_link_options(moldcast PUBLIC
        -fsanitize=address,undefined
    )
endif()

# ---------------- executables ----------------

add_executable(sender tools/sender.cpp)
target_link_libraries(sender PRIVATE moldcast)

add_executable(receiver tools/receiver.cpp)
target_link_libraries(receiver PRIVATE moldcast)


# ---------------- tests ----------------

option(MOLDCAST_BUILD_TESTS "Build unit tests" ON)

if(MOLDCAST_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

