#pragma once
#include "core/gfx/render_texture.h"
#include "modules/mod_architect/canvas.h"

namespace convoy { namespace architect {

class LivePreview {
public:
    ~LivePreview() { shutdown(); }
    void initialize();
    void shutdown();
    void render(bool* open, const Canvas* canvas);
    void set_enable_crt(bool v) { crt_enabled_ = v; }
    void set_native_res(int w, int h) { native_w_ = w; native_h_ = h; }

private:
    void update_source_texture(const Canvas* canvas);
    void apply_crt_pass();
    void compile_crt_shader();

    RenderTexture source_rt_;
    RenderTexture crt_rt_;
    uint32_t      crt_program_ = 0;
    uint32_t      quad_vao_    = 0;
    uint32_t      quad_vbo_    = 0;
    int           native_w_    = 320;
    int           native_h_    = 180;
    bool          crt_enabled_ = true;
    bool          initialized_ = false;
};

}}
