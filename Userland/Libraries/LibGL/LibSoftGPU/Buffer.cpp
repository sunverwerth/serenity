#include "Buffer.h"

namespace SoftGPU
{

void Buffer::upload_data(size_t offset, void const* data, size_t size)
{
    m_data.overwrite(offset, data, size);
}

}