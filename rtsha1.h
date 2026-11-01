#pragma once

#include <cstdint>
#include <cstddef>

class rtsha1 {
public:
    void calc(const void* data, size_t length, uint8_t* outDigest);
    void toHexString(const uint8_t* digest, char* outHex);
};