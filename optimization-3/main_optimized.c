#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <immintrin.h>
#include <openssl/evp.h>

void print_hash(unsigned char *hash, unsigned int length) {
    for (unsigned int i = 0; i < length; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
}

// Função de rotação à direita com intrinsics
static inline __m256i rotr(__m256i x, int n) {
    return _mm256_or_si256(_mm256_srli_epi32(x, n), _mm256_slli_epi32(x, 32 - n));
}

// Função para realizar uma única rodada de SHA-256 com intrinsics
void sha256_transform_intrinsics(__m256i state[8], const unsigned char data[64]) {
    // Constantes K do SHA-256
    static const unsigned int K[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
        0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
        0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };
    
    __m256i W[64];
    __m256i a, b, c, d, e, f, g, h;
    int i;

    // Carregar os dados e preparar o array W
    for (i = 0; i < 16; i++) {
        W[i] = _mm256_set1_epi32(((unsigned int*)data)[i]);
    }

    for (i = 16; i < 64; i++) {
        __m256i s0 = _mm256_xor_si256(rotr(W[i-15], 7), rotr(W[i-15], 18));
        s0 = _mm256_xor_si256(s0, _mm256_srli_epi32(W[i-15], 3));
        __m256i s1 = _mm256_xor_si256(rotr(W[i-2], 17), rotr(W[i-2], 19));
        s1 = _mm256_xor_si256(s1, _mm256_srli_epi32(W[i-2], 10));
        W[i] = _mm256_add_epi32(W[i-16], s0);
        W[i] = _mm256_add_epi32(W[i], W[i-7]);
        W[i] = _mm256_add_epi32(W[i], s1);
    }

    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];
    f = state[5];
    g = state[6];
    h = state[7];

    for (i = 0; i < 64; i++) {
        __m256i S1 = _mm256_xor_si256(rotr(e, 6), rotr(e, 11));
        S1 = _mm256_xor_si256(S1, rotr(e, 25));
        __m256i ch = _mm256_xor_si256(_mm256_and_si256(e, f), _mm256_andnot_si256(e, g));
        __m256i temp1 = _mm256_add_epi32(h, S1);
        temp1 = _mm256_add_epi32(temp1, ch);
        temp1 = _mm256_add_epi32(temp1, _mm256_set1_epi32(K[i]));
        temp1 = _mm256_add_epi32(temp1, W[i]);
        __m256i S0 = _mm256_xor_si256(rotr(a, 2), rotr(a, 13));
        S0 = _mm256_xor_si256(S0, rotr(a, 22));
        __m256i maj = _mm256_xor_si256(_mm256_and_si256(a, b), _mm256_and_si256(a, c));
        maj = _mm256_xor_si256(maj, _mm256_and_si256(b, c));
        __m256i temp2 = _mm256_add_epi32(S0, maj);
        h = g;
        g = f;
        f = e;
        e = _mm256_add_epi32(d, temp1);
        d = c;
        c = b;
        b = a;
        a = _mm256_add_epi32(temp1, temp2);
    }

    state[0] = _mm256_add_epi32(state[0], a);
    state[1] = _mm256_add_epi32(state[1], b);
    state[2] = _mm256_add_epi32(state[2], c);
    state[3] = _mm256_add_epi32(state[3], d);
    state[4] = _mm256_add_epi32(state[4], e);
    state[5] = _mm256_add_epi32(state[5], f);
    state[6] = _mm256_add_epi32(state[6], g);
    state[7] = _mm256_add_epi32(state[7], h);
}

int main() {
    // String de entrada
    const char *input = "Texto para ser hashado";
    
    // Buffer para armazenar o hash
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_length;

    // Contexto de mensagem
    EVP_MD_CTX *mdctx;
    if((mdctx = EVP_MD_CTX_new()) == NULL) {
        printf("Erro ao criar contexto de mensagem.\n");
        return 1;
    }
    
    // Calcular SHA-256
    if(1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL)) {
        printf("Erro ao inicializar digest.\n");
        return 1;
    }
    if(1 != EVP_DigestUpdate(mdctx, input, strlen(input))) {
        printf("Erro ao atualizar digest.\n");
        return 1;
    }
    if(1 != EVP_DigestFinal_ex(mdctx, hash, &hash_length)) {
        printf("Erro ao finalizar digest.\n");
        return 1;
    }
    
    // Limpar contexto
    EVP_MD_CTX_free(mdctx);
    
    // Imprimir o hash
    printf("Hash SHA-256: ");
    print_hash(hash, hash_length);
    
    return 0;
}
