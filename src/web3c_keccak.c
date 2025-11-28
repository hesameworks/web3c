#include "web3c/keccak.h"

#include <string.h>

/* Internal rotation macro */
#define WEB3C_ROTL64(x, n) (((x) << (n)) | ((x) >> (64 - (n))))

/* Keccak-f[1600] round constants */
static const uint64_t web3c_keccakf_rndc[24] = {
    0x0000000000000001ULL, 0x0000000000008082ULL,
    0x800000000000808aULL, 0x8000000080008000ULL,
    0x000000000000808bULL, 0x0000000080000001ULL,
    0x8000000080008081ULL, 0x8000000000008009ULL,
    0x000000000000008aULL, 0x0000000000000088ULL,
    0x0000000080008009ULL, 0x000000008000000aULL,
    0x000000008000808bULL, 0x800000000000008bULL,
    0x8000000000008089ULL, 0x8000000000008003ULL,
    0x8000000000008002ULL, 0x8000000000000080ULL,
    0x000000000000800aULL, 0x800000008000000aULL,
    0x8000000080008081ULL, 0x8000000000008080ULL,
    0x0000000080000001ULL, 0x8000000080008008ULL
};

/* Rotation offsets (flattened) */
static const unsigned int web3c_keccakf_rotc[24] = {
     1,  3,  6, 10, 15, 21,
    28, 36, 45, 55,  2, 14,
    27, 41, 56,  8, 25, 43,
    62, 18, 39, 61, 20, 44
};

/* Permutation indices */
static const unsigned int web3c_keccakf_piln[24] = {
    10,  7, 11, 17, 18,  3,
     5, 16,  8, 21, 24,  4,
    15, 23, 19, 13, 12,  2,
    20, 14, 22,  9,  6,  1
};

static uint64_t web3c_load64_le(const uint8_t *src)
{
    uint64_t w = 0;
    for (int i = 0; i < 8; ++i) {
        w |= ((uint64_t)src[i]) << (8 * i);
    }
    return w;
}

static void web3c_store64_le(uint8_t *dst, uint64_t w)
{
    for (int i = 0; i < 8; ++i) {
        dst[i] = (uint8_t)(w >> (8 * i));
    }
}

/**
 * @brief Core Keccak-f[1600] permutation.
 *
 * This is the 24-round permutation used by all Keccak-based functions.
 */
static void web3c_keccakf(uint64_t st[25])
{
    int round;
    uint64_t bc[5];

    for (round = 0; round < 24; ++round) {
        /* Theta */
        for (int i = 0; i < 5; ++i) {
            bc[i] = st[i] ^ st[i + 5] ^ st[i + 10] ^ st[i + 15] ^ st[i + 20];
        }

        for (int i = 0; i < 5; ++i) {
            uint64_t t = bc[(i + 4) % 5] ^ WEB3C_ROTL64(bc[(i + 1) % 5], 1);
            for (int j = 0; j < 25; j += 5) {
                st[j + i] ^= t;
            }
        }

        /* Rho and Pi */
        uint64_t t = st[1];
        for (int i = 0; i < 24; ++i) {
            int j = web3c_keccakf_piln[i];
            uint64_t tmp = st[j];
            st[j] = WEB3C_ROTL64(t, web3c_keccakf_rotc[i]);
            t = tmp;
        }

        /* Chi */
        for (int j = 0; j < 25; j += 5) {
            uint64_t a0 = st[j + 0];
            uint64_t a1 = st[j + 1];
            uint64_t a2 = st[j + 2];
            uint64_t a3 = st[j + 3];
            uint64_t a4 = st[j + 4];

            st[j + 0] ^= (~a1) & a2;
            st[j + 1] ^= (~a2) & a3;
            st[j + 2] ^= (~a3) & a4;
            st[j + 3] ^= (~a4) & a0;
            st[j + 4] ^= (~a0) & a1;
        }

        /* Iota */
        st[0] ^= web3c_keccakf_rndc[round];
    }
}

void web3c_keccak256_init(web3c_keccak_ctx *ctx)
{
    if (!ctx) {
        return;
    }

    memset(ctx->state, 0, sizeof(ctx->state));
    memset(ctx->buffer, 0, sizeof(ctx->buffer));
    ctx->rate       = 136;  /* 1088 bits */
    ctx->buffer_pos = 0;
    ctx->finalized  = 0;
}

void web3c_keccak_update(web3c_keccak_ctx *ctx,
                         const uint8_t *data,
                         size_t len)
{
    if (!ctx || !data || len == 0 || ctx->finalized) {
        return;
    }

    while (len > 0) {
        size_t space = ctx->rate - ctx->buffer_pos;
        size_t to_copy = (len < space) ? len : space;

        memcpy(ctx->buffer + ctx->buffer_pos, data, to_copy);
        ctx->buffer_pos += to_copy;
        data += to_copy;
        len  -= to_copy;

        if (ctx->buffer_pos == ctx->rate) {
            /* Absorb full block into state */
            for (size_t i = 0; i < ctx->rate / 8; ++i) {
                uint64_t lane = web3c_load64_le(ctx->buffer + 8 * i);
                ctx->state[i] ^= lane;
            }
            web3c_keccakf(ctx->state);
            ctx->buffer_pos = 0;
        }
    }
}

void web3c_keccak_final(web3c_keccak_ctx *ctx, uint8_t out[32])
{
    if (!ctx || !out || ctx->finalized) {
        return;
    }

    /* Keccak (Ethereum) padding: pad10*1, i.e. 0x01 ... 0x80 */
    ctx->buffer[ctx->buffer_pos++] = 0x01;

    while (ctx->buffer_pos < ctx->rate) {
        ctx->buffer[ctx->buffer_pos++] = 0x00;
    }

    ctx->buffer[ctx->rate - 1] |= 0x80;

    /* Absorb final padded block */
    for (size_t i = 0; i < ctx->rate / 8; ++i) {
        uint64_t lane = web3c_load64_le(ctx->buffer + 8 * i);
        ctx->state[i] ^= lane;
    }
    web3c_keccakf(ctx->state);

    /* Squeeze first 32 bytes (256 bits) */
    for (size_t i = 0; i < 4; ++i) {
        web3c_store64_le(out + 8 * i, ctx->state[i]);
    }

    ctx->finalized = 1;
}

int web3c_keccak256(const uint8_t *data, size_t len, uint8_t out[32])
{
    if (!out) {
        return -1;
    }

    web3c_keccak_ctx ctx;
    web3c_keccak256_init(&ctx);

    if (data && len > 0) {
        web3c_keccak_update(&ctx, data, len);
    }

    web3c_keccak_final(&ctx, out);
    return 0;
}