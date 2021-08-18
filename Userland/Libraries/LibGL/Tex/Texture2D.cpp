/*
 * Copyright (c) 2021, Jesse Buhagiar <jooster669@gmail.com>
 * Copyright (c) 2021, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Format.h>
#include <LibGL/GL/gl.h>
#include <LibGL/Tex/Texture2D.h>
#include <string.h>

namespace GL {

static void blit(const void* src, int src_w, int src_h, void* dst, int x_offset, int y_offset, int dst_w, int dst_h, GLenum src_format)
{
    (void)(dst_h);

    const u8* src_data = reinterpret_cast<const u8*>(src);
    u32* dst_data = reinterpret_cast<u32*>(dst);

    for (int y = y_offset; y < y_offset + src_h; y++) {
        for (int x = x_offset; x < x_offset + src_w; x++) {
            int r = 0;
            int g = 0;
            int b = 0;
            int a = 0;
            
            switch (src_format) {
            case GL_RGBA:
                r = *src_data++;
                g = *src_data++;
                b = *src_data++;
                a = *src_data++;
                break;
            case GL_BGRA:
                b = *src_data++;
                g = *src_data++;
                r = *src_data++;
                a = *src_data++;
                break;
            case GL_RGB:
                r = *src_data++;
                g = *src_data++;
                b = *src_data++;
                a = 255;
                break;
            case GL_BGR:
                b = *src_data++;
                g = *src_data++;
                r = *src_data++;
                a = 255;
                break;
            default:
                VERIFY_NOT_REACHED();
            }

            dst_data[y * dst_w + x] = a << 24 | r << 16 | g << 8 | b;
        }
    }
}

void Texture2D::upload_texture_data(GLenum, GLint lod, GLint internal_format, GLsizei width, GLsizei height, GLint, GLenum format, GLenum type, const GLvoid* pixels)
{
    VERIFY(type == GL_UNSIGNED_BYTE);

    // NOTE: Some target, format, and internal formats are currently unsupported.
    // Considering we control this library, and `gl.h` itself, we don't need to add any
    // checks here to see if we support them; the program will simply fail to compile..

    auto& mip = m_mipmaps[lod];
    mip.set_width(width);
    mip.set_height(height);
    mip.pixel_data().resize(width * height);

    // No pixel data was supplied. Just allocate texture memory and leave it uninitialized.
    if (pixels == nullptr) {
        return;
    }

    m_internal_format = internal_format;

    blit(pixels, width, height, mip.pixel_data().data(), 0, 0, width, height, format);
}

void Texture2D::replace_sub_texture_data(GLint lod, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* data)
{
    VERIFY(type == GL_UNSIGNED_BYTE);

    auto& mip = m_mipmaps[lod];
    if (mip.height() < 1 || mip.width() < 1) {
        return;
    }

    blit(data, width, height, mip.pixel_data().data(), xoffset, yoffset, mip.width(), mip.height(), format);
}

MipMap const& Texture2D::mipmap(unsigned lod) const
{
    if (lod >= m_mipmaps.size())
        return m_mipmaps.at(m_mipmaps.size() - 1);

    return m_mipmaps.at(lod);
}

}
