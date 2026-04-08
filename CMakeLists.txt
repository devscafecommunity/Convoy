cmake_minimum_required(VERSION 3.20)
project(Convoy VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include custom find modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/cmake")

# Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/v1.13.0.zip
)
FetchContent_MakeAvailable(googletest)

# Core library (math, types, no graphics dependencies)
add_library(convoy_core)
target_include_directories(convoy_core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Try to find graphics dependencies (optional for core)
find_package(GLFW3 3.4 QUIET)
find_package(OpenGL QUIET)
find_package(GLAD QUIET)
find_package(ImGui QUIET)

# Only add window_manager if graphics deps are found
if(GLFW3_FOUND AND OpenGL_FOUND AND GLAD_FOUND AND ImGui_FOUND)
  target_sources(convoy_core PRIVATE
    src/core/window_manager.cpp
    src/core/event_bus.cpp
  )
  target_link_libraries(convoy_core PUBLIC
    GLFW::GLFW
    OpenGL::OpenGL
    GLAD::GLAD
    ImGui::ImGui
  )
else()
  # Minimal core without graphics
  target_sources(convoy_core PRIVATE
    src/core/event_bus.cpp
  )
endif()

# CVP format and filesystem I/O
target_sources(convoy_core PRIVATE
  src/core/cvp_format.cpp
  src/core/filesystem.cpp
  src/core/command_manager.cpp
  src/core/commands/pixel_command.cpp
)

# Architect Module Library (no graphics deps)
add_library(mod_architect
  src/modules/mod_architect/canvas.cpp
  src/modules/mod_architect/viewport.cpp
  src/modules/mod_architect/architect_ui.cpp
  src/modules/mod_architect/tools/pencil_tool.cpp
  src/modules/mod_architect/tools/eraser_tool.cpp
  src/modules/mod_architect/tools/bucket_tool.cpp
)
target_include_directories(mod_architect PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(mod_architect PUBLIC convoy_core)

# Main Executable (only if graphics deps available)
if(GLFW3_FOUND AND OpenGL_FOUND AND GLAD_FOUND AND ImGui_FOUND)
  add_executable(convoy
    src/convoy/main.cpp
  )
  target_link_libraries(convoy PRIVATE convoy_core mod_architect)
endif()

# Compiler flags
if(MSVC)
  target_compile_options(convoy_core PRIVATE /W4 /WX)
  if(TARGET convoy)
    target_compile_options(convoy PRIVATE /W4 /WX)
  endif()
else()
  target_compile_options(convoy_core PRIVATE -Wall -Wextra -Wpedantic -Werror)
  if(TARGET convoy)
    target_compile_options(convoy PRIVATE -Wall -Wextra -Wpedantic -Werror)
  endif()
endif()

# Testing
enable_testing()
add_subdirectory(tests)

