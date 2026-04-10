# Codex Asset File (.CAF) Format Design

> **Status:** Approved
> **Version:** 1.0
> **Purpose:** Optimized binary asset format for direct loading by Caffeine Engine

## Overview

CAF is a zero-parsing binary format designed for fast loading via memcpy. Pixel data is GPU-ready, and collision metadata is embedded as a bitmap layer ("intelligent pixels").

---

## File Structure

```
┌─────────────────────────────────────────────────────┐
│ HEADER (32 bytes)                                    │
├─────────────────────────────────────────────────────┤
│ METADATA (32 bytes)                                  │
├─────────────────────────────────────────────────────┤
│ COLLISION MAP (variable)                            │
├─────────────────────────────────────────────────────┤
│ PIXEL DATA (variable)                               │
└─────────────────────────────────────────────────────┘
```

All sections are padded to 32-byte alignment for CPU cache efficiency.

---

## Header (32 bytes)

| Offset | Size | Field | Description |
|--------|------|-------|-------------|
| 0 | 4 | magic | "CAF1" (0x43414631) |
| 4 | 2 | version | Format version (1) |
| 6 | 2 | type | 0=Sprite, 1=Tilemap, 2=Animation |
| 8 | 4 | file_size | Total file size in bytes |
| 12 | 4 | data_offset | Offset to pixel data |
| 16 | 4 | colmap_offset | Offset to collision map |
| 20 | 4 | width | Canvas width in pixels |
| 24 | 4 | height | Canvas height in pixels |
| 28 | 4 | reserved | Padding for alignment |

---

## Metadata (32 bytes)

| Offset | Size | Field | Description |
|--------|------|-------|-------------|
| 0 | 2 | pivot_x | Anchor point X (relative to top-left) |
| 2 | 2 | pivot_y | Anchor point Y |
| 4 | 2 | width | Canvas width (duplicate for convenience) |
| 6 | 2 | height | Canvas height (duplicate) |
| 8 | 4 | colmap_size | Collision map size in bytes |
| 12 | 4 | pixel_data_size | Pixel data size in bytes |
| 16 | 4 | animation_frame_count | Number of frames (if type=Animation) |
| 20 | 4 | animation_fps | Frames per second |
| 24 | 8 | reserved | Padding |

---

## Collision Map

**Format:** 2 bits per pixel

| Value | Meaning |
|-------|---------|
| 00 | Empty (no collision) |
| 01 | Solid (wall, floor) |
| 10 | Trigger (event zone) |
| 11 | Hurtbox (damage zone) |

**Size Calculation:** `(width × height + 3) / 4` bytes (rounded up to 32-byte boundary)

**Storage:** Bit-packed, LSB first within each byte

---

## Pixel Data

**Format:** Raw RGBA8 (8 bits per channel)

- Order: R, G, B, A (native for OpenGL)
- Directly usable with `glTexImage2D` without conversion
- Aligned to 32-byte boundary

---

## Export Pipeline

1. Flatten all canvas layers to single RGBA buffer
2. Generate collision map from collision layer (if exists)
3. Write header with calculated offsets
4. Write metadata
5. Write collision map (padded to 32-byte boundary)
6. Write pixel data (padded to 32-byte boundary)

---

## Usage in Caffeine Engine

```cpp
// Pseudo-code for fast loading
void load_caf(const char* path) {
    File file(path);
    CAF_Header* header = file.read<CAF_Header>();
    
    // Direct GPU upload (no parsing)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                header->width, header->height, 0,
                GL_RGBA, GL_UNSIGNED_BYTE,
                file.seek(header->data_offset));
    
    // Collision data ready for physics system
    CollisionMap* colmap = file.seek(header->colmap_offset);
}
```

---

## Design Decisions

1. **2-bit collision map:** Allows 4 collision types per pixel - sufficient for most 2D games
2. **32-byte alignment:** Optimizes CPU cache line reads
3. **No compression in v1:** Raw RGBA for fastest load. LZ4 can be added in v2 if needed
4. **Duplicate dimensions in header+metadata:** Enables single-read validation

---

**Approved by:** User
**Date:** 2026-04-09