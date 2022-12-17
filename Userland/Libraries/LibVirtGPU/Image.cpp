/*
 * Copyright (c) 2022, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibVirtGPU/Image.h>

namespace VirtGPU {

Image::Image(void const* ownership_token, GPU::PixelFormat const& pixel_format, u32 width, u32 height, u32 depth, u32 max_levels)
    : GPU::Image(ownership_token, pixel_format, width, height, depth, max_levels)
{    
    dbgln("LibVirtGPU: Creating new image with dimensions {}x{}x{} and {} mip levels", width, height, depth, max_levels);
}

Image::~Image()
{
    dbgln("LibVirtGPU: Destroying image with dimensions {}x{}x{} and {} mip levels", width_at_level(0), height_at_level(0), depth_at_level(0), number_of_levels());
}

void Image::regenerate_mipmaps()
{    
}

void Image::write_texels(u32, Vector3<i32> const&, void const*, GPU::ImageDataLayout const&)
{
}

void Image::read_texels(u32, Vector3<i32> const&, void*, GPU::ImageDataLayout const&) const
{
}

void Image::copy_texels(GPU::Image const&, u32, Vector3<u32> const&, Vector3<u32> const&, u32, Vector3<u32> const&)
{
}

}
