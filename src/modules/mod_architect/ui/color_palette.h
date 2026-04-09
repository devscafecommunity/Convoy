#pragma once
#include "shared/color.h"
#include <vector>
#include <string>
#include <functional>

namespace convoy { namespace architect {

struct IndexedPalette {
    std::string        name;
    std::vector<Color> colors;
    static IndexedPalette make_default_16();
    static IndexedPalette make_db32();
};

class ColorPalette {
public:
    using SelectFn = std::function<void(Color)>;

    void render(bool* open = nullptr);
    void load_palette(const IndexedPalette& pal) { palette_ = pal; }
    void set_callback(SelectFn fn) { on_select_ = std::move(fn); }
    Color selected() const { return selected_; }

private:
    IndexedPalette palette_ = IndexedPalette::make_default_16();
    Color          selected_{255, 255, 255, 255};
    int            sel_idx_ = -1;
    SelectFn       on_select_;
};

}}
