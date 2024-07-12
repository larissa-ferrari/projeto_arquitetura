#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>

#define DATA_SIZE 1024 * 1024 // 1 MB de dados

// Definições necessárias para o algoritmo SHA-256
typedef unsigned int SHA_LONG;
#define SHA256_CBLOCK (SHA_LBLOCK*4)

#define DECLARE_IS_ENDIAN \
    int endian = 1; \
    unsigned char *p = (unsigned char *)&endian; \
    int little = (*p == 1)

#define IS_LITTLE_ENDIAN (little)

// Funções de hash originais
void sha256_block_data_order_original(SHA256_CTX *ctx, const void *in, size_t num) {
    // Função original do OpenSSL
    const unsigned char *data = in;
    SHA256_Update(ctx, data, num * SHA256_CBLOCK);
}

// Funções de hash otimizadas
void sha256_block_data_order_optimized(SHA256_CTX *ctx, const void *in, size_t num);

// Benchmarking
void benchmark(const char *label, void (*hash_func)(SHA256_CTX *, const void *, size_t), const void *data, size_t len) {
    SHA256_CTX ctx;
    clock_t start, end;
    double cpu_time_used;

    // Inicialize o contexto SHA-256
    SHA256_Init(&ctx);

    // Marque o início do tempo
    start = clock();

    // Execute a função de hash
    hash_func(&ctx, data, len / SHA256_CBLOCK);

    // Marque o fim do tempo
    end = clock();

    // Calcule o tempo de CPU usado
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    // Imprima o tempo usado
    printf("%s: %f seconds\n", label, cpu_time_used);
}

int main() {
    // Dados de entrada para o benchmark
    unsigned char *data = malloc(DATA_SIZE);

    if (data == NULL) {
        fprintf(stderr, "Erro ao alocar memória.\n");
        return 1;
    }

    // Preencha os dados com algum valor (por exemplo, zeros)
    memset(data, 0, DATA_SIZE);

    // Execute o benchmark para a implementação original
    benchmark("Original", sha256_block_data_order_original, data, DATA_SIZE);

    // Execute o benchmark para a implementação otimizada
    benchmark("Otimizada", sha256_block_data_order_optimized, data, DATA_SIZE);

    // Libere a memória alocada
    free(data);

    return 0;
}

// Função otimizada sha256_block_data_order_optimized
void sha256_block_data_order_optimized(SHA256_CTX *ctx, const void *in, size_t num) {
    SHA_LONG a, b, c, d, e, f, g, h, s0, s1, T1;
    SHA_LONG X[16];
    int i;
    const unsigned char *data = in;
    DECLARE_IS_ENDIAN;

    while (num--) {
        a = ctx->h[0];
        b = ctx->h[1];
        c = ctx->h[2];
        d = ctx->h[3];
        e = ctx->h[4];
        f = ctx->h[5];
        g = ctx->h[6];
        h = ctx->h[7];

        if (!IS_LITTLE_ENDIAN && sizeof(SHA_LONG) == 4 && ((size_t)in % 4) == 0) {
            const SHA_LONG *W = (const SHA_LONG *)data;

            T1 = X[0] = W[0]; // ROUND_00_15(0, a, b, c, d, e, f, g, h);
            T1 = X[1] = W[1]; // ROUND_00_15(1, h, a, b, c, d, e, f, g);
            T1 = X[2] = W[2]; // ROUND_00_15(2, g, h, a, b, c, d, e, f);
            T1 = X[3] = W[3]; // ROUND_00_15(3, f, g, h, a, b, c, d, e);
            T1 = X[4] = W[4]; // ROUND_00_15(4, e, f, g, h, a, b, c, d);
            T1 = X[5] = W[5]; // ROUND_00_15(5, d, e, f, g, h, a, b, c);
            T1 = X[6] = W[6]; // ROUND_00_15(6, c, d, e, f, g, h, a, b);
            T1 = X[7] = W[7]; // ROUND_00_15(7, b, c, d, e, f, g, h, a);
            T1 = X[8] = W[8]; // ROUND_00_15(8, a, b, c, d, e, f, g, h);
            T1 = X[9] = W[9]; // ROUND_00_15(9, h, a, b, c, d, e, f, g);
            T1 = X[10] = W[10]; // ROUND_00_15(10, g, h, a, b, c, d, e, f);
            T1 = X[11] = W[11]; // ROUND_00_15(11, f, g, h, a, b, c, d, e);
            T1 = X[12] = W[12]; // ROUND_00_15(12, e, f, g, h, a, b, c, d);
            T1 = X[13] = W[13]; // ROUND_00_15(13, d, e, f, g, h, a, b, c);
            T1 = X[14] = W[14]; // ROUND_00_15(14, c, d, e, f, g, h, a, b);
            T1 = X[15] = W[15]; // ROUND_00_15(15, b, c, d, e, f, g, h, a);

            data += SHA256_CBLOCK;
        } else {
            SHA_LONG l;

            (void)HOST_c2l(data, l); T1 = X[0] = l; // ROUND_00_15(0, a, b, c, d, e, f, g, h);
            (void)HOST_c2l(data, l); T1 = X[1] = l; // ROUND_00_15(1, h, a, b, c, d, e, f, g);
            (void)HOST_c2l(data, l); T1 = X[2] = l; // ROUND_00_15(2, g, h, a, b, c, d, e, f);
            (void)HOST_c2l(data, l); T1 = X[3] = l; // ROUND_00_15(3, f, g, h, a, b, c, d, e);
            (void)HOST_c2l(data, l); T1 = X[4] = l; // ROUND_00_15(4, e, f, g, h, a, b, c, d);
            (void)HOST_c2l(data, l); T1 = X[5] = l; // ROUND_00_15(5, d, e, f, g, h, a, b, c);
            (void)HOST_c2l(data, l); T1 = X[6] = l; // ROUND_00_15(6, c, d, e, f, g, h, a, b);
            (void)HOST_c2l(data, l); T1 = X[7] = l; // ROUND_00_15(7, b, c, d, e, f, g, h, a);
            (void)HOST_c2l(data, l); T1 = X[8] = l; // ROUND_00_15(8, a, b, c, d, e, f, g, h);
            (void)HOST_c2l(data, l); T1 = X[9] = l; // ROUND_00_15(9, h, a, b, c, d, e, f, g);
            (void)HOST_c2l(data, l); T1 = X[10] = l; // ROUND_00_15(10, g, h, a, b, c, d, e, f);
            (void)HOST_c2l(data, l); T1 = X[11] = l; // ROUND_00_15(11, f, g, h, a, b, c, d, e);
            (void)HOST_c2l(data, l); T1 = X[12] = l; // ROUND_00_15(12, e, f, g, h, a, b, c, d);
            (void)HOST_c2l(data, l); T1 = X[13] = l; // ROUND_00_15(13, d, e, f, g, h, a, b, c);
            (void)HOST_c2l(data, l); T1 = X[14] = l; // ROUND_00_15(14, c, d, e, f, g, h, a, b);
            (void)HOST_c2l(data, l); T1 = X[15] = l; // ROUND_00_15(15, b, c, d, e, f, g, h, a);
        }

        for (i = 16; i < 64; i++) {
            s0 = X[(i + 1) & 0x0f];
            s0 = sigma0(s0);
            s1 = X[(i + 14) & 0x0f];
            s1 = sigma1(s1);

            T1 = X[i & 0x0f] += s0 + s1 + X[(i + 9) & 0x0f];
            ROUND_16_63(i, a, b, c, d, e, f, g, h);
        }

        ctx->h[0] += a;
        ctx->h[1] += b;
        ctx->h[2] += c;
        ctx->h[3] += d;
        ctx->h[4] += e;
        ctx->h[5] += f;
        ctx->h[6] += g;
        ctx->h[7] += h;
    }
}
