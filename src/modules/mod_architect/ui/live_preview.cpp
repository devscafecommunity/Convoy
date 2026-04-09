#include "live_preview.h"
#include <imgui.h>
#include <glad/glad.h>
#include <vector>

namespace convoy { namespace architect {

static const char* kCRTVert = R"(
#version 410 core
layout(location=0) in vec2 aPos;
layout(location=1) in vec2 aUV;
out vec2 vUV;
void main() { vUV = aUV; gl_Position = vec4(aPos, 0.0, 1.0); }
)";

static const char* kCRTFrag = R"(
#version 410 core
in  vec2 vUV;
out vec4 FragColor;
uniform sampler2D uTex;
uniform vec2      uResolution;

void main() {
    vec4 col = texture(uTex, vUV);
    float scanline = sin(vUV.y * uResolution.y * 3.14159) * 0.5 + 0.5;
    scanline = mix(1.0, scanline, 0.18);
    col.rgb *= vec3(0.92, 1.0, 0.88);
    col.rgb *= scanline;
    vec2 uv2  = vUV * 2.0 - 1.0;
    float vig = 1.0 - dot(uv2, uv2) * 0.25;
    col.rgb  *= vig;
    FragColor = col;
}
)";

static uint32_t compile_shader(GLenum type, const char* src) {
    uint32_t s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    return s;
}

void LivePreview::compile_crt_shader() {
    uint32_t vert = compile_shader(GL_VERTEX_SHADER,   kCRTVert);
    uint32_t frag = compile_shader(GL_FRAGMENT_SHADER, kCRTFrag);
    crt_program_ = glCreateProgram();
    glAttachShader(crt_program_, vert);
    glAttachShader(crt_program_, frag);
    glLinkProgram(crt_program_);
    glDeleteShader(vert);
    glDeleteShader(frag);
}

void LivePreview::initialize() {
    if (initialized_) return;

    source_rt_.create(native_w_, native_h_);
    crt_rt_.create(native_w_, native_h_);

    compile_crt_shader();

    float verts[] = {
        -1,-1, 0,0,   1,-1, 1,0,   1,1, 1,1,
        -1,-1, 0,0,   1,1,  1,1,  -1,1, 0,1
    };
    glGenVertexArrays(1, &quad_vao_);
    glGenBuffers(1, &quad_vbo_);
    glBindVertexArray(quad_vao_);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float),
                          (void*)(2*sizeof(float)));
    glBindVertexArray(0);

    initialized_ = true;
}

void LivePreview::shutdown() {
    if (!initialized_) return;
    source_rt_.destroy();
    crt_rt_.destroy();
    if (crt_program_) { glDeleteProgram(crt_program_); crt_program_ = 0; }
    if (quad_vao_)    { glDeleteVertexArrays(1, &quad_vao_); quad_vao_ = 0; }
    if (quad_vbo_)    { glDeleteBuffers(1, &quad_vbo_);      quad_vbo_ = 0; }
    initialized_ = false;
}

void LivePreview::update_source_texture(const Canvas* canvas) {
    if (!canvas) return;
    std::vector<uint32_t> pixels;
    const_cast<Canvas*>(canvas)->composite_to_texture(pixels);

    glBindTexture(GL_TEXTURE_2D, source_rt_.texture_id());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                 static_cast<int>(canvas->width()),
                 static_cast<int>(canvas->height()),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void LivePreview::apply_crt_pass() {
    crt_rt_.bind();
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(crt_program_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, source_rt_.texture_id());
    glUniform1i(glGetUniformLocation(crt_program_, "uTex"), 0);
    glUniform2f(glGetUniformLocation(crt_program_, "uResolution"),
                static_cast<float>(native_w_),
                static_cast<float>(native_h_));

    glBindVertexArray(quad_vao_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    crt_rt_.unbind();
}

void LivePreview::render(bool* open, const Canvas* canvas) {
    if (!initialized_) initialize();
    if (!ImGui::Begin("Live Preview##Architect", open)) { ImGui::End(); return; }

    ImGui::Checkbox("CRT Filter", &crt_enabled_);
    ImGui::SameLine();
    ImGui::TextDisabled("(%dx%d native)", native_w_, native_h_);

    if (canvas) {
        update_source_texture(canvas);
        if (crt_enabled_) apply_crt_pass();

        uint32_t display_tex = crt_enabled_
            ? crt_rt_.texture_id()
            : source_rt_.texture_id();

        ImVec2 avail = ImGui::GetContentRegionAvail();
        float  ar    = static_cast<float>(native_w_) / static_cast<float>(native_h_);
        float  w     = avail.x;
        float  h     = w / ar;
        if (h > avail.y) { h = avail.y; w = h * ar; }

        ImGui::Image((ImTextureID)(intptr_t)display_tex, {w, h},
                     {0,1}, {1,0});
    } else {
        ImGui::TextDisabled("No canvas");
    }

    ImGui::End();
}

}}
