#pragma once
#include <cstdint>

namespace convoy {

class RenderTexture {
public:
    RenderTexture() = default;
    ~RenderTexture() { destroy(); }

    RenderTexture(const RenderTexture&)            = delete;
    RenderTexture& operator=(const RenderTexture&) = delete;

    void create(int width, int height);
    void resize(int width, int height);
    void destroy();

    void bind();
    void unbind();

    uint32_t texture_id() const { return texture_id_; }
    int width()    const { return width_; }
    int height()   const { return height_; }
    bool is_valid() const { return fbo_id_ != 0; }

private:
    uint32_t fbo_id_     = 0;
    uint32_t texture_id_ = 0;
    uint32_t rbo_id_     = 0;
    int      width_      = 0;
    int      height_     = 0;
};

}
