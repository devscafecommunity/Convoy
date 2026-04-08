find_package(OpenGL REQUIRED)

add_library(GLAD INTERFACE)
target_include_directories(GLAD INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/include)
target_sources(GLAD INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/src/glad.c)

add_library(GLAD::GLAD ALIAS GLAD)
