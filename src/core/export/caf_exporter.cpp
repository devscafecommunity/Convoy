#include "core/export/caf_exporter.h"

#include <fstream>

namespace convoy
{

bool CAFExporter::export_to_file(const char* filepath,
                                 const uint8_t* pixel_data,
                                 uint16_t width,
                                 uint16_t height,
                                 const uint8_t* collision_data,
                                 uint16_t collision_width,
                                 uint16_t collision_height)
{
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open())
    {
        return false;
    }

    CAFHeader header;
    header.width = width;
    header.height = height;
    header.data_offset = sizeof(CAFHeader);

    if (collision_data && collision_width > 0 && collision_height > 0)
    {
        header.collision_width = collision_width;
        header.collision_height = collision_height;
        header.collision_offset = header.data_offset + (width * height * 4);
    }

    file.write(reinterpret_cast<const char*>(&header), sizeof(header));

    file.write(reinterpret_cast<const char*>(pixel_data), width * height * 4);

    if (collision_data && collision_width > 0 && collision_height > 0)
    {
        file.write(reinterpret_cast<const char*>(collision_data), collision_width * collision_height);
    }

    file.close();
    return true;
}

}  // namespace convoy
