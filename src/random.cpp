#include "random.h"

PRNG prng;

uint64_t PRNG::rand64() {
    uint64_t tmp = keys_[0];
    keys_[0] += rotate(keys_[1] ^ 0xc5462216u ^ (static_cast<uint64_t>(0xcf14f4ebu) << 32), 1);
    return keys_[1] += rotate(tmp ^ 0x75ecfc58u ^ (static_cast<uint64_t>(0x9576080cu) << 32), 9);
}

void PRNG::seedRandom(uint64_t seed) {
    keys_[0] = seed;
    keys_[1] = seed;
    for (int i = 0; i < 64; i++) {
        rand64();
    }
}

uint64_t PRNG::randMagic() {
    return rand64() & rand64() & rand64();
}
