#ifndef CONVOY_CAF_EXPORTER_H
#define CONVOY_CAF_EXPORTER_H

#include <cstdint>
#include <cstring>

namespace convoy
{

// CAF Format: Convoy Art File
// Binary format for pixel art with embedded collision metadata
#pragma pack(push, 1)
struct CAFHeader
{
    char magic[4];              // "CAF\0"
    uint16_t version;           // Format version (currently 1)
    uint16_t width;             // Image width in pixels
    uint16_t height;            // Image height in pixels
    uint32_t data_offset;       // Offset to pixel data
    uint32_t meta_offset;       // Offset to metadata chunk (0 if none)
    uint32_t collision_offset;  // Offset to collision data (0 if none)
    uint16_t collision_width;   // Collision box width (0 if none)
    uint16_t collision_height;  // Collision box height (0 if none)
    uint8_t reserved[12];       // Reserved for future use

    CAFHeader()
    {
        magic[0] = 'C';
        magic[1] = 'A';
        magic[2] = 'F';
        magic[3] = '\0';
        version = 1;
        width = 0;
        height = 0;
        data_offset = 0;
        meta_offset = 0;
        collision_offset = 0;
        collision_width = 0;
        collision_height = 0;
        std::memset(reserved, 0, sizeof(reserved));
    }
};

struct CAFMetadata
{
    uint16_t width;
    uint16_t height;
    int16_t pivot_x;
    int16_t pivot_y;
    uint8_t flags;
    uint8_t reserved[7];

    CAFMetadata() : width(0), height(0), pivot_x(0), pivot_y(0), flags(0)
    {
        std::memset(reserved, 0, sizeof(reserved));
    }
};
#pragma pack(pop)

class CAFExporter
{
   public:
    // Export RGBA8 pixel data to CAF file
    // collision_data: optional binary collision mask (1 byte per pixel, 0=passable, 1=solid)
    static bool export_to_file(const char* filepath,
                               const uint8_t* pixel_data,
                               uint16_t width,
                               uint16_t height,
                               const uint8_t* collision_data,
                               uint16_t collision_width,
                               uint16_t collision_height);
};

}  // namespace convoy

#endif
