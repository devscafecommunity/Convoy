enable_testing()

add_executable(convoy_tests
  test_math.cpp
)

target_link_libraries(convoy_tests PRIVATE convoy_core gtest gtest_main)

target_include_directories(convoy_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

add_test(NAME Math COMMAND convoy_tests --gtest_filter=MathTest.*:ColorTest.*)

