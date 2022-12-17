#include <LibGPU/Image.h>

namespace GPU {

Image::Image(void const* ownership_token, PixelFormat const& pixel_format, u32 width, u32 height, u32 depth, u32 max_levels)
    : m_ownership_token{ownership_token}
    , m_pixel_format{pixel_format}
    , m_width{width}
    , m_height{height}
    , m_depth{depth}
    , m_number_of_levels{max_levels}
{
    VERIFY(width > 0);
    VERIFY(height > 0);
    VERIFY(depth > 0);
    VERIFY(max_levels > 0);

    u32 maximum_number_of_levels = AK::max(AK::max(AK::log2(width), AK::log2(height)), AK::log2(depth)) + 1u;
    m_number_of_levels = AK::min(m_number_of_levels, maximum_number_of_levels);

    for (u32 i = 0; i < m_number_of_levels; i++) {
        m_mipmap_sizes.append({width, height, depth});
        width = AK::max(width / 2, 1);
        height = AK::max(height / 2, 1);
        depth = AK::max(height / 2, 1);
    }
}

}
