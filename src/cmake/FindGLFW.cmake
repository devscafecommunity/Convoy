find_package(GLFW3 CONFIG QUIET)
if(NOT GLFW3_FOUND)
  find_package(GLFW3 3.4 REQUIRED)
endif()
