#include "filesystem.h"
#include "cvp_format.h"
#include "logger.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace convoy {

bool FileSystem::project_exists(const std::string& path) {
    return fs::exists(path) && fs::is_directory(path);
}

bool FileSystem::create_project_directory(const std::string& path) {
    try {
        fs::create_directories(path);
        Logger::info("Created project directory: {}", path);
        return true;
    } catch (const std::exception& e) {
        Logger::error("Failed to create project directory: {}", e.what());
        return false;
    }
}

std::string FileSystem::get_canvas_path(const std::string& project_path) {
    return (fs::path(project_path) / "canvas.cvp").string();
}

bool FileSystem::save_canvas(const std::string& project_path, const architect::Canvas* canvas) {
    if (!project_exists(project_path)) {
        if (!create_project_directory(project_path)) {
            return false;
        }
    }
    
    std::string canvas_path = get_canvas_path(project_path);
    return CVPFormat::save_to_file(canvas_path, canvas);
}

bool FileSystem::load_canvas(const std::string& project_path, architect::Canvas* canvas) {
    std::string canvas_path = get_canvas_path(project_path);
    if (!fs::exists(canvas_path)) {
        Logger::error("Canvas file not found: {}", canvas_path);
        return false;
    }
    
    return CVPFormat::load_from_file(canvas_path, canvas);
}

} // namespace convoy
