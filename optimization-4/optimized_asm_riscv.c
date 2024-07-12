#if defined(__GNUC__) && __GNUC__>=2 && !defined(OPENSSL_NO_ASM) && !defined(OPENSSL_NO_INLINE_ASM)
# if defined(__riscv_zknh)
#  define Sigma0(x) ({ MD32_REG_T ret; asm ("sha256sum0 %0, %1" : "=r"(ret) : "r"(x)); ret; })
#  define Sigma1(x) ({ MD32_REG_T ret; asm ("sha256sum1 %0, %1" : "=r"(ret) : "r"(x)); ret; })
#  define sigma0(x) ({ MD32_REG_T ret; asm ("sha256sig0 %0, %1" : "=r"(ret) : "r"(x)); ret; })
#  define sigma1(x) ({ MD32_REG_T ret; asm ("sha256sig1 %0, %1" : "=r"(ret) : "r"(x)); ret; })
# endif
# if defined(__riscv_zbt) || defined(__riscv_zpn)
#  define Ch(x,y,z) ({ MD32_REG_T ret; asm (".insn r4 0x33, 1, 0x3, %0, %2, %1, %3" : "=r"(ret) : "r"(x), "r"(y), "r"(z)); ret; })
#  define Maj(x,y,z) ({ MD32_REG_T ret; asm (".insn r4 0x33, 1, 0x3, %0, %2, %1, %3" : "=r"(ret) : "r"(x^z), "r"(y), "r"(x)); ret; })
# endif
#else
# define Sigma0(x) (ROTATE((x),30) ^ ROTATE((x),19) ^ ROTATE((x),10))
# define Sigma1(x) (ROTATE((x),26) ^ ROTATE((x),21) ^ ROTATE((x),7))
# define sigma0(x) (ROTATE((x),25) ^ ROTATE((x),14) ^ ((x)>>3))
# define sigma1(x) (ROTATE((x),15) ^ ROTATE((x),13) ^ ((x)>>10))
# define Ch(x,y,z) (((x) & (y)) ^ ((~(x)) & (z)))
# define Maj(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#endif

#define ROUND_00_15(i,a,b,c,d,e,f,g,h) do { \
    T1 += h + Sigma1(e) + Ch(e,f,g) + K256[i]; \
    h = Sigma0(a) + Maj(a,b,c); \
    d += T1; h += T1; \
} while (0)

#define ROUND_16_63(i,a,b,c,d,e,f,g,h,X) do { \
    s0 = X[(i+1)&0x0f]; s0 = sigma0(s0); \
    s1 = X[(i+14)&0x0f]; s1 = sigma1(s1); \
    T1 = X[(i)&0x0f] += s0 + s1 + X[(i+9)&0x0f]; \
    ROUND_00_15(i,a,b,c,d,e,f,g,h); \
} while (0)

static void sha256_block_data_order(SHA256_CTX *ctx, const void *in, size_t num)
{
    unsigned MD32_REG_T a, b, c, d, e, f, g, h, s0, s1, T1;
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

            T1 = X[0] = W[0]; ROUND_00_15(0, a, b, c, d, e, f, g, h);
            T1 = X[1] = W[1]; ROUND_00_15(1, h, a, b, c, d, e, f, g);
            T1 = X[2] = W[2]; ROUND_00_15(2, g, h, a, b, c, d, e, f);
            T1 = X[3] = W[3]; ROUND_00_15(3, f, g, h, a, b, c, d, e);
            T1 = X[4] = W[4]; ROUND_00_15(4, e, f, g, h, a, b, c, d);
            T1 = X[5] = W[5]; ROUND_00_15(5, d, e, f, g, h, a, b, c);
            T1 = X[6] = W[6]; ROUND_00_15(6, c, d, e, f, g, h, a, b);
            T1 = X[7] = W[7]; ROUND_00_15(7, b, c, d, e, f, g, h, a);
            T1 = X[8] = W[8]; ROUND_00_15(8, a, b, c, d, e, f, g, h);
            T1 = X[9] = W[9]; ROUND_00_15(9, h, a, b, c, d, e, f, g);
            T1 = X[10] = W[10]; ROUND_00_15(10, g, h, a, b, c, d, e, f);
            T1 = X[11] = W[11]; ROUND_00_15(11, f, g, h, a, b, c, d, e);
            T1 = X[12] = W[12]; ROUND_00_15(12, e, f, g, h, a, b, c, d);
            T1 = X[13] = W[13]; ROUND_00_15(13, d, e, f, g, h, a, b, c);
            T1 = X[14] = W[14]; ROUND_00_15(14, c, d, e, f, g, h, a, b);
            T1 = X[15] = W[15]; ROUND_00_15(15, b, c, d, e, f, g, h, a);

            data += SHA256_CBLOCK;
        } else {
            SHA_LONG l;

            (void)HOST_c2l(data, l); T1 = X[0] = l; ROUND_00_15(0, a, b, c, d, e, f, g, h);
            (void)HOST_c2l(data, l); T1 = X[1] = l; ROUND_00_15(1, h, a, b, c, d, e, f, g);
            (void)HOST_c2l(data, l); T1 = X[2] = l; ROUND_00_15(2, g, h, a, b, c, d, e, f);
            (void)HOST_c2l(data, l); T1 = X[3] = l; ROUND_00_15(3, f, g, h, a, b, c, d, e);
            (void)HOST_c2l(data, l); T1 = X[4] = l; ROUND_00_15(4, e, f, g, h, a, b, c, d);
            (void)HOST_c2l(data, l); T1 = X[5] = l; ROUND_00_15(5, d, e, f, g, h, a, b, c);
            (void)HOST_c2l(data, l); T1 = X[6] = l; ROUND_00_15(6, c, d, e, f, g, h, a, b);
            (void)HOST_c2l(data, l); T1 = X[7] = l; ROUND_00_15(7, b, c, d, e, f, g, h, a);
            (void)HOST_c2l(data, l); T1 = X[8] = l; ROUND_00_15(8, a, b, c, d, e, f, g, h);
            (void)HOST_c2l(data, l); T1 = X[9] = l; ROUND_00_15(9, h, a, b, c, d, e, f, g);
            (void)HOST_c2l(data, l); T1 = X[10] = l; ROUND_00_15(10, g, h, a, b, c, d, e, f);
            (void)HOST_c2l(data, l); T1 = X[11] = l; ROUND_00_15(11, f, g, h, a, b, c, d, e);
            (void)HOST_c2l(data, l); T1 = X[12] = l; ROUND_00_15(12, e, f, g, h, a, b, c, d);
            (void)HOST_c2l(data, l); T1 = X[13] = l; ROUND_00_15(13, d, e, f, g, h, a, b, c);
            (void)HOST_c2l(data, l); T1 = X[14] = l; ROUND_00_15(14, c, d, e, f, g, h, a, b);
            (void)HOST_c2l(data, l); T1 = X[15] = l; ROUND_00_15(15, b, c, d, e, f, g, h, a);
        }

        for (i = 16; i < 64; i += 8) {
            ROUND_16_63(i+0, a, b, c, d, e, f, g, h, X);
            ROUND_16_63(i+1, h, a, b, c, d, e, f, g, X);
            ROUND_16_63(i+2, g, h, a, b, c, d, e, f, X);
            ROUND_16_63(i+3, f, g, h, a, b, c, d, e, X);
            ROUND_16_63(i+4, e, f, g, h, a, b, c, d, X);
            ROUND_16_63(i+5, d, e, f, g, h, a, b, c, X);
            ROUND_16_63(i+6, c, d, e, f, g, h, a, b, X);
            ROUND_16_63(i+7, b, c, d, e, f, g, h, a, X);
        }

        ctx->h[0] += a;
        ctx->h[1] += b;
        ctx->h[2] += c;
        ctx->h[3] += d;
        ctx->h[4] += e;
        ctx->h[5] += f;
        ctx->h[6] += g;
        ctx->h[7] += h;

        OPENSSL_cleanse((void *)X, sizeof(X));
    }
}
