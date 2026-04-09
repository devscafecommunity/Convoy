set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui")

# Guard: ImGui depends on GLFW and OpenGL already being found
if(NOT GLFW3_FOUND OR NOT OpenGL_FOUND OR NOT GLAD_FOUND)
  set(ImGui_FOUND FALSE)
  return()
endif()

if(EXISTS "${IMGUI_DIR}/imgui.h" AND
   EXISTS "${IMGUI_DIR}/backends/imgui_impl_opengl3.h" AND
   EXISTS "${IMGUI_DIR}/backends/imgui_impl_glfw.h")
  if(NOT TARGET ImGui)
    add_library(ImGui STATIC
      "${IMGUI_DIR}/imgui.cpp"
      "${IMGUI_DIR}/imgui_demo.cpp"
      "${IMGUI_DIR}/imgui_draw.cpp"
      "${IMGUI_DIR}/imgui_widgets.cpp"
      "${IMGUI_DIR}/imgui_tables.cpp"
      "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
      "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
    )
    target_include_directories(ImGui PUBLIC "${IMGUI_DIR}")
    target_include_directories(ImGui PRIVATE "${IMGUI_DIR}/backends")
    target_link_libraries(ImGui PUBLIC OpenGL::GL glfw GLAD::GLAD)
    # Define custom loader so imgl3w is not used
    target_compile_definitions(ImGui PRIVATE IMGUI_IMPL_OPENGL_LOADER_CUSTOM)
    add_library(ImGui::ImGui ALIAS ImGui)
  endif()
  set(ImGui_FOUND TRUE)
else()
  set(ImGui_FOUND FALSE)
endif()
