/*
 * Copyright (c) 2021, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/String.h>

namespace HAL::GPU {

struct DeviceInfo {
    String vendor;
    String model;
    bool is_accelerated;
    unsigned num_texture_units;
};

}