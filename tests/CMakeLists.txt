enable_testing()

add_executable(convoy_tests
  test_math.cpp
  test_window_manager.cpp
)

target_link_libraries(convoy_tests PRIVATE convoy_core gtest gtest_main)

target_include_directories(convoy_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

add_test(NAME AllTests COMMAND convoy_tests)

