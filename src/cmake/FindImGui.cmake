set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui")

# Guard: ImGui depends on GLFW and OpenGL already being found
if(NOT GLFW3_FOUND OR NOT OpenGL_FOUND)
  set(ImGui_FOUND FALSE)
  return()
endif()

if(EXISTS "${IMGUI_DIR}/imgui.h" AND
   EXISTS "${IMGUI_DIR}/backends/imgui_impl_opengl3.h" AND
   EXISTS "${IMGUI_DIR}/backends/imgui_impl_glfw.h")
  if(NOT TARGET ImGui)
    add_library(ImGui INTERFACE)
    target_include_directories(ImGui INTERFACE "${IMGUI_DIR}")
    target_sources(ImGui INTERFACE
      "${IMGUI_DIR}/imgui.cpp"
      "${IMGUI_DIR}/imgui_demo.cpp"
      "${IMGUI_DIR}/imgui_draw.cpp"
      "${IMGUI_DIR}/imgui_widgets.cpp"
      "${IMGUI_DIR}/imgui_tables.cpp"
      "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
      "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
    )
    target_link_libraries(ImGui INTERFACE OpenGL::OpenGL GLFW::GLFW)
    add_library(ImGui::ImGui ALIAS ImGui)
  endif()
  set(ImGui_FOUND TRUE)
else()
  set(ImGui_FOUND FALSE)
endif()
