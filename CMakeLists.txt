cmake_minimum_required(VERSION 3.20)
project(Convoy VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/cmake")

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/v1.13.0.zip
)
FetchContent_MakeAvailable(googletest)

add_library(convoy_core)
target_include_directories(convoy_core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

find_package(GLFW3 3.4 QUIET)
find_package(OpenGL QUIET)
find_package(GLAD QUIET)
find_package(ImGui QUIET)

if(GLFW3_FOUND AND OpenGL_FOUND AND GLAD_FOUND AND ImGui_FOUND)
  target_sources(convoy_core PRIVATE
    src/core/window_manager.cpp
    src/core/event_bus.cpp
    src/core/ui/theme_manager.cpp
    src/core/ui/dockspace_manager.cpp
    src/core/ui/main_menu_bar.cpp
    src/core/input/input_handler.cpp
    src/core/gfx/render_texture.cpp
  )
  target_link_libraries(convoy_core PUBLIC
    glfw
    OpenGL::GL
    GLAD::GLAD
    ImGui::ImGui
  )
else()
  target_sources(convoy_core PRIVATE
    src/core/event_bus.cpp
  )
endif()

target_sources(convoy_core PRIVATE
  src/core/cvp_format.cpp
  src/core/filesystem.cpp
  src/core/command_manager.cpp
  src/core/commands/pixel_command.cpp
  src/core/plugins/plugin_registry.cpp
  src/core/plugins/hook_system.cpp
)

add_library(mod_architect
  src/modules/mod_architect/canvas.cpp
  src/modules/mod_architect/viewport.cpp
  src/modules/mod_architect/tools/pencil_tool.cpp
  src/modules/mod_architect/tools/eraser_tool.cpp
  src/modules/mod_architect/tools/bucket_tool.cpp
  src/modules/mod_architect/tools/pivot_tool.cpp
  src/modules/mod_architect/tools/hitbox_tool.cpp
)
target_include_directories(mod_architect PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(mod_architect PUBLIC convoy_core)

if(GLFW3_FOUND AND OpenGL_FOUND AND GLAD_FOUND AND ImGui_FOUND)
  target_sources(mod_architect PRIVATE
    src/modules/mod_architect/architect_ui.cpp
    src/modules/mod_architect/ui/color_palette.cpp
    src/modules/mod_architect/ui/dod_visualizer.cpp
    src/modules/mod_architect/ui/live_preview.cpp
  )
endif()

if(GLFW3_FOUND AND OpenGL_FOUND AND GLAD_FOUND AND ImGui_FOUND)
  add_library(mod_walker src/modules/mod_walker/walker_ui.cpp)
  target_include_directories(mod_walker PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
  target_link_libraries(mod_walker PUBLIC convoy_core)

  add_library(mod_forge src/modules/mod_forge/forge_ui.cpp)
  target_include_directories(mod_forge PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
  target_link_libraries(mod_forge PUBLIC convoy_core)

  add_library(mod_sequencer src/modules/mod_sequencer/sequencer_ui.cpp)
  target_include_directories(mod_sequencer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
  target_link_libraries(mod_sequencer PUBLIC convoy_core)
endif()

if(GLFW3_FOUND AND OpenGL_FOUND AND GLAD_FOUND AND ImGui_FOUND)
  add_executable(convoy
    src/convoy/main.cpp
  )
  target_link_libraries(convoy PRIVATE convoy_core mod_architect mod_walker mod_forge mod_sequencer)
endif()

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

enable_testing()
add_subdirectory(tests)
