#pragma once

namespace convoy { namespace walker {

class WalkerUI {
public:
    void render();

private:
    void render_tile_grid_placeholder();
    void render_tile_selector();

    bool show_collision_ = false;
    int  grid_size_      = 16;
};

}}
