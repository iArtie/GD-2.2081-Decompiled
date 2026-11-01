#include "Platform/ThirdParty/rtsha1.h"

#include <cstring>

namespace {

constexpr uint32_t kInitialState[5] = {
    0x67452301,
    0xEFCDAB89,
    0x98BADCFE,
    0x10325476,
    0xC3D2E1F0
};

inline uint32_t rotl(uint32_t value, int bits)
{
    return (value << bits) | (value >> (32 - bits));
}

void processBlock(uint32_t state[5], const uint8_t* block)
{
    uint32_t w[80];

    for (int i = 0; i < 16; i++)
    {
        w[i] = ((uint32_t)block[i * 4] << 24)
             | ((uint32_t)block[i * 4 + 1] << 16)
             | ((uint32_t)block[i * 4 + 2] << 8)
             | (uint32_t)block[i * 4 + 3];
    }

    for (int i = 16; i < 80; i++)
        w[i] = rotl(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);

    uint32_t a = state[0];
    uint32_t b = state[1];
    uint32_t c = state[2];
    uint32_t d = state[3];
    uint32_t e = state[4];

    for (int i = 0; i < 80; i++)
    {
        uint32_t f;
        uint32_t k;

        if (i < 20)
        {
            f = (b & c) | ((~b) & d);
            k = 0x5A827999;
        }
        else if (i < 40)
        {
            f = b ^ c ^ d;
            k = 0x6ED9EBA1;
        }
        else if (i < 60)
        {
            f = (b & c) | (b & d) | (c & d);
            k = 0x8F1BBCDC;
        }
        else
        {
            f = b ^ c ^ d;
            k = 0xCA62C1D6;
        }

        uint32_t temp = rotl(a, 5) + f + e + k + w[i];
        e = d;
        d = c;
        c = rotl(b, 30);
        b = a;
        a = temp;
    }

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
}

}

void rtsha1::calc(const void* data, size_t length, uint8_t* outDigest)
{
    uint32_t state[5];
    std::memcpy(state, kInitialState, sizeof(state));

    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    size_t fullBlocks = length / 64;

    for (size_t i = 0; i < fullBlocks; i++)
        processBlock(state, bytes + i * 64);

    size_t remaining = length - fullBlocks * 64;
    uint8_t tail[128] = {0};
    std::memcpy(tail, bytes + fullBlocks * 64, remaining);
    tail[remaining] = 0x80;

    size_t tailBlocks = (remaining < 56) ? 1 : 2;
    uint64_t bitLength = (uint64_t)length * 8;

    for (int i = 0; i < 8; i++)
        tail[tailBlocks * 64 - 1 - i] = (uint8_t)(bitLength >> (i * 8));

    for (size_t i = 0; i < tailBlocks; i++)
        processBlock(state, tail + i * 64);

    for (int i = 0; i < 20; i++)
        outDigest[i] = (uint8_t)(state[i / 4] >> (8 * (3 - (i % 4))));
}

void rtsha1::toHexString(const uint8_t* digest, char* outHex)
{
    static const char kHexDigits[] = "0123456789abcdef";

    for (int i = 0; i < 20; i++)
    {
        outHex[i * 2] = kHexDigits[digest[i] >> 4];
        outHex[i * 2 + 1] = kHexDigits[digest[i] & 0x0F];
    }

    outHex[40] = '\0';
}