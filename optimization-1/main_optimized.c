#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <immintrin.h> // Para intrinsics Intel

void print_hash(unsigned char hash[SHA256_DIGEST_LENGTH]) {
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
}

int main() {
    const char *input = "Texto para ser hashado";
    unsigned char hash[SHA256_DIGEST_LENGTH];
    
    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    size_t len = strlen(input);
    size_t remaining = len;
    const unsigned char *data = (const unsigned char*) input;

    while (remaining >= 64) {
        __m128i block[4];
        for (int i = 0; i < 4; i++) {
            block[i] = _mm_loadu_si128((__m128i*)(data + i * 16));
        }

        __m128i state[8];
        for (int i = 0; i < 8; i++) {
            state[i] = _mm_loadu_si128((__m128i*)&sha256.h[i * 4]);
        }

        for (int i = 0; i < 64; i += 16) {
            state[0] = _mm_sha256rnds2_epu32(state[0], state[4], block[i / 16]);
            state[1] = _mm_sha256rnds2_epu32(state[1], state[5], block[i / 16]);
            state[2] = _mm_sha256rnds2_epu32(state[2], state[6], block[i / 16]);
            state[3] = _mm_sha256rnds2_epu32(state[3], state[7], block[i / 16]);
        }

        for (int i = 0; i < 8; i++) {
            _mm_storeu_si128((__m128i*)&sha256.h[i * 4], state[i]);
        }

        data += 64;
        remaining -= 64;
    }

    if (remaining > 0) {
        SHA256_Update(&sha256, data, remaining);
    }

    SHA256_Final(hash, &sha256);

    printf("Hash SHA-256: ");
    print_hash(hash);

    return 0;
}