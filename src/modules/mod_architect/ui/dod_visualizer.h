#pragma once
#include "modules/mod_architect/canvas.h"
#include <cstdint>

namespace convoy { namespace architect {

struct AlignmentInfo {
    size_t data_bytes;
    size_t aligned_bytes;
    bool   is_aligned;
    int    waste_bytes;
};

class DODVisualizer {
public:
    void render(bool* open, const Canvas* canvas);

    static AlignmentInfo compute_alignment(const Canvas* canvas) {
        if (!canvas) return {};
        size_t bytes = static_cast<size_t>(canvas->width()) *
                       static_cast<size_t>(canvas->height()) * 4;
        AlignmentInfo info;
        info.data_bytes    = bytes;
        info.is_aligned    = (bytes % 32 == 0);
        info.aligned_bytes = info.is_aligned ? bytes : bytes + (32 - bytes % 32);
        info.waste_bytes   = static_cast<int>(info.aligned_bytes - bytes);
        return info;
    }

    static size_t compute_vram_estimate(const Canvas* canvas) {
        if (!canvas) return 0;
        size_t per_layer = static_cast<size_t>(canvas->width()) *
                           static_cast<size_t>(canvas->height()) * 4;
        return per_layer * canvas->get_layers().size();
    }

private:
    void render_memory_bar(size_t used_bytes, size_t total_bytes);
    void render_alignment_badge(const AlignmentInfo& info);
};

}}
