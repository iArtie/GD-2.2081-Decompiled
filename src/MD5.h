#pragma once

#include <stdint.h>

class MD5 {
public:
    static void MD5Transform(uint32_t state[4], const unsigned char block[64]);
};