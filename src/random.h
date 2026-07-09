#pragma once

#include <cstdint>

class PRNG {
public:
    PRNG() : keys_{} {}

    uint64_t rand64();
    void seedRandom(uint64_t seed);
    uint64_t randMagic();

private:
    static uint64_t rotate(uint64_t v, uint8_t s) {
        return (v >> s) | (v << (64 - s));
    }

    uint64_t keys_[2];
};

extern PRNG prng;
