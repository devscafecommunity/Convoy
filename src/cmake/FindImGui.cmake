add_library(ImGui INTERFACE)
target_include_directories(ImGui INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui)
target_sources(ImGui INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui_demo.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui_draw.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui_widgets.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui_tables.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_opengl3.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_glfw.cpp
)
target_link_libraries(ImGui INTERFACE OpenGL::OpenGL GLFW::GLFW)

add_library(ImGui::ImGui ALIAS ImGui)
