cmake_minimum_required(VERSION 3.15)
project(DriveSim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Collect sources from src/ (the intended layout); also fall back to the
# project root so the build still works if the files are ever flattened.
file(GLOB SOURCES CONFIGURE_DEPENDS "src/*.cpp" "*.cpp")
add_executable(drivesim ${SOURCES})

if(MSVC)
    target_compile_options(drivesim PRIVATE /W4)
else()
    target_compile_options(drivesim PRIVATE -Wall -Wextra -Wpedantic)
endif()
