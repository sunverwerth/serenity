/*
 * Copyright (c) 2021, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/ByteBuffer.h>
#include <AK/Format.h>
#include <LibHAL/GPU/Texture2D.h>

namespace SoftGPU {

class Texture2D final : public HAL::GPU::Texture2D {
public:
    virtual void update_subimage(size_t, int, int, size_t, size_t, void const*, HAL::GPU::SurfaceDescription const&) override { }
};

}