#pragma once
#include "shared/types.h"
#include "modules/mod_architect/canvas.h"
#include <vector>
#include <string>
#include <cstdint>

namespace convoy {

#pragma pack(push, 1)
struct CVPHeader {
    char magic[4];
    uint16_t version;
    uint16_t layer_count;
    uint32_t canvas_width;
    uint32_t canvas_height;
};
#pragma pack(pop)

class CVPFormat {
public:
    static bool save_to_file(const std::string& path, const architect::Canvas* canvas);
    static bool load_from_file(const std::string& path, architect::Canvas* canvas);
    
private:
    static constexpr const char* MAGIC = "CVPF";
    static constexpr uint16_t VERSION = 1;
    static constexpr size_t HEADER_SIZE = sizeof(CVPHeader);
};

} // namespace convoy
