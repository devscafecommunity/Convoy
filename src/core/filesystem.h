#pragma once
#include "modules/mod_architect/canvas.h"
#include <string>

namespace convoy {

class FileSystem {
public:
    static bool project_exists(const std::string& path);
    static bool create_project_directory(const std::string& path);
    
    static bool save_canvas(const std::string& project_path, const architect::Canvas* canvas);
    static bool load_canvas(const std::string& project_path, architect::Canvas* canvas);
    
    static std::string get_canvas_path(const std::string& project_path);
};

} // namespace convoy
