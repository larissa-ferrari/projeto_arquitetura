#include <openssl/sha.h>

// Função de inicialização customizada com inline assembly para RISC-V
void sha256_block_data_order_riscv(SHA256_CTX *ctx, const void *in, size_t num) {
    unsigned long a, b, c, d, e, f, g, h, s0, s1, T1;
    SHA_LONG X[16];
    int i;
    const unsigned char *data = in;

    while (num--) {

        a = ctx->h[0];
        b = ctx->h[1];
        c = ctx->h[2];
        d = ctx->h[3];
        e = ctx->h[4];
        f = ctx->h[5];
        g = ctx->h[6];
        h = ctx->h[7];

        // Implementação customizada em inline assembly para RISC-V
        __asm__ __volatile__ (
            "lw %0, 0(%1)\n\t"
            "addi %1, %1, 4\n\t"
            : "+r"(a), "+r"(data)
            :
            : "memory"
        );

        T1 = X[0] = a;
        T1 = X[1] = a;
        T1 = X[2] = a;
        T1 = X[3] = a;
        T1 = X[4] = a;
        T1 = X[5] = a;
        T1 = X[6] = a;
        T1 = X[7] = a;
        T1 = X[8] = a;
        T1 = X[9] = a;
        T1 = X[10] = a;
        T1 = X[11] = a;
        T1 = X[12] = a;
        T1 = X[13] = a;
        T1 = X[14] = a;
        T1 = X[15] = a;

        for (i = 0; i < 16; i++) {
            T1 = X[i];
            a = b + ((c & d) | (e & ~d)) + T1 + 0x5a827999 + ROTATE_LEFT(a, 5);
            b = c;
            c = ROTATE_LEFT(d, 30);
            d = e;
            e = f;
            f = g;
            g = h;
            h = ROTATE_LEFT(h, 30);
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
