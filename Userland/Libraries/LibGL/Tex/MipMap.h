/*
 * Copyright (c) 2021, Jesse Buhagiar <jooster669@gmail.com>
 * Copyright (c) 2021, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Vector.h>
#include <LibGL/GL/gl.h>
#include <LibGfx/Vector4.h>

namespace GL {

class MipMap {
public:
    MipMap() = default;
    ~MipMap() {
        delete[] m_pixel_data;
    }

    void set_width(GLsizei width) { m_width = width; }
    void set_height(GLsizei height) { m_height = height; }
    GLsizei width() const { return m_width; }
    GLsizei height() const { return m_height; }

    u32*& pixel_data() { return m_pixel_data; }

    FloatVector4 texel(unsigned x, unsigned y) const
    {
        if (x != lastx || y != lasty) {
            lastx = x;
            lasty = y;
        
            constexpr float div = 1 / 255.f;
            u32 texel = m_pixel_data[y * m_width + x];
            lastcol = {
                ((texel >> 16) & 0xff) * div,
                ((texel >> 8) & 0xff) * div,
                (texel & 0xff) * div,
                ((texel >> 24) & 0xff) * div
            };
        }

        return lastcol;
    }

private:
mutable unsigned lastx=-1;
mutable unsigned lasty=-1;
mutable FloatVector4 lastcol;
    GLsizei m_width { 0 };
    GLsizei m_height { 0 };
    u32* m_pixel_data { nullptr };
};
}
