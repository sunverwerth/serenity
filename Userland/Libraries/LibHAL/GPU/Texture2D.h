/*
 * Copyright (c) 2021, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/RefCounted.h>

namespace HAL::GPU {

enum class PixelFormat {
    R_8,
    R_16,
    RGB_565,
    RGB_8888,
    RGBA_8888,
    RGBX_8888,
    RGB_F32F32F32,
    RGBA_F32F32F32,
};

struct SurfaceDescription {
    PixelFormat format;
    size_t width;
    size_t height;
    size_t row_stride;
    size_t element_stride;
    bool big_endian;
};

class Texture2D : public RefCounted<Texture2D> {
public:
    virtual ~Texture2D() { }

    virtual void update_subimage(size_t level, int x, int y, size_t width, size_t height, void const* data, SurfaceDescription const&) = 0;
};

}