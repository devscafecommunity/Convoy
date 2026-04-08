#include "cvp_format.h"
#include "logger.h"
#include <fstream>
#include <cstring>
#include <stdexcept>

namespace convoy {

bool CVPFormat::save_to_file(const std::string& path, const architect::Canvas* canvas) {
    if (!canvas) {
        return false;
    }
    
    try {
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) {
            Logger::error("Failed to open file for writing: {}", path);
            return false;
        }
        
        CVPHeader header;
        std::memcpy(header.magic, "CVPF", 4);
        header.version = 1;
        header.layer_count = static_cast<uint16_t>(canvas->get_layers().size());
        header.canvas_width = canvas->width();
        header.canvas_height = canvas->height();
        
        file.write(reinterpret_cast<const char*>(&header), HEADER_SIZE);
        
        for (const auto& layer : canvas->get_layers()) {
            uint8_t name_len = static_cast<uint8_t>(std::min(size_t(255), layer.name.size()));
            file.write(reinterpret_cast<const char*>(&name_len), 1);
            file.write(layer.name.c_str(), name_len);
            
            uint8_t flags = (layer.visible ? 1 : 0) | (layer.locked ? 2 : 0);
            file.write(reinterpret_cast<const char*>(&flags), 1);
            
            file.write(reinterpret_cast<const char*>(&layer.opacity), sizeof(float));
            
            size_t pixel_count = canvas->width() * canvas->height();
            file.write(reinterpret_cast<const char*>(layer.pixels.data()), pixel_count * 4);
        }
        
        file.close();
        Logger::info("Saved canvas to: {}", path);
        return true;
    } catch (const std::exception& e) {
        Logger::error("Exception while saving CVP file: {}", e.what());
        return false;
    }
}

bool CVPFormat::load_from_file(const std::string& path, architect::Canvas* canvas) {
    if (!canvas) {
        return false;
    }
    
    try {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            Logger::error("Failed to open file for reading: {}", path);
            return false;
        }
        
        CVPHeader header;
        file.read(reinterpret_cast<char*>(&header), HEADER_SIZE);
        
        if (std::strncmp(header.magic, "CVPF", 4) != 0) {
            Logger::error("Invalid CVP file magic");
            return false;
        }
        
        if (header.version != 1) {
            Logger::error("Unsupported CVP version: {}", header.version);
            return false;
        }
        
        if (canvas->width() != header.canvas_width || canvas->height() != header.canvas_height) {
            *canvas = architect::Canvas(header.canvas_width, header.canvas_height);
        }
        
        for (uint16_t i = 0; i < header.layer_count; ++i) {
            uint8_t name_len;
            file.read(reinterpret_cast<char*>(&name_len), 1);
            
            std::string name(name_len, '\0');
            if (name_len > 0) {
                file.read(&name[0], name_len);
            }
            
            uint8_t flags;
            file.read(reinterpret_cast<char*>(&flags), 1);
            
            bool visible = (flags & 1) != 0;
            bool locked = (flags & 2) != 0;
            
            float opacity;
            file.read(reinterpret_cast<char*>(&opacity), sizeof(float));
            
            if (i == 0) {
                canvas->get_layers()[0].name = name;
            } else {
                canvas->add_layer(name);
            }
            
            Layer& layer = canvas->get_layers()[i];
            layer.visible = visible;
            layer.locked = locked;
            layer.opacity = opacity;
            
            size_t pixel_count = header.canvas_width * header.canvas_height;
            file.read(reinterpret_cast<char*>(layer.pixels.data()), pixel_count * 4);
        }
        
        file.close();
        Logger::info("Loaded canvas from: {}", path);
        return true;
    } catch (const std::exception& e) {
        Logger::error("Exception while loading CVP file: {}", e.what());
        return false;
    }
}

} // namespace convoy
