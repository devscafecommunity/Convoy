enable_testing()

set(TEST_SOURCES
  test_math.cpp
  test_canvas.cpp
  test_cvp_format.cpp
  test_command_manager.cpp
  test_viewport_zoom.cpp
  test_tools.cpp
  test_plugin_registry.cpp
  test_hook_system.cpp
  test_dod_visualizer.cpp
  integration_test.cpp
)

find_package(GLFW3 3.4 QUIET)
find_package(OpenGL QUIET)
if(GLFW3_FOUND AND OpenGL_FOUND)
  list(APPEND TEST_SOURCES test_window_manager.cpp test_input_handler.cpp test_architect_ui.cpp)
endif()

add_executable(convoy_tests ${TEST_SOURCES})

target_link_libraries(convoy_tests PRIVATE convoy_core mod_architect gtest gtest_main)

target_include_directories(convoy_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

add_test(NAME AllTests COMMAND convoy_tests)
