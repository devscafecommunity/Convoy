cmake_minimum_required(VERSION 3.20)
project(Convoy VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include custom find modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/cmake")

# Dependencies
find_package(GLFW3 3.4 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLAD REQUIRED)
find_package(ImGui REQUIRED)

# Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/v1.13.0.zip
)
FetchContent_MakeAvailable(googletest)

# Convoy Core Library (headers only for math, color, types)
add_library(convoy_core
  src/core/window_manager.cpp
  src/core/event_bus.cpp
)

target_include_directories(convoy_core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(convoy_core
  GLFW::GLFW
  OpenGL::OpenGL
  GLAD::GLAD
  ImGui::ImGui
)

# Main Executable
add_executable(convoy
  src/convoy/main.cpp
)

target_link_libraries(convoy PRIVATE convoy_core)

# Compiler flags
if(MSVC)
  target_compile_options(convoy_core PRIVATE /W4 /WX)
  target_compile_options(convoy PRIVATE /W4 /WX)
else()
  target_compile_options(convoy_core PRIVATE -Wall -Wextra -Wpedantic -Werror)
  target_compile_options(convoy PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# Testing
enable_testing()
add_subdirectory(tests)
