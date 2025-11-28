#include "web3c/rlp.h"

#include <string.h>

static int rlp_write_length(size_t len,
                            uint8_t short_base,
                            uint8_t long_base,
                            uint8_t *out,
                            size_t out_size,
                            size_t *out_len)
{
    if (len <= 55) {
        size_t needed = 1;
        if (out_len) {
            *out_len = needed;
        }
        if (out == NULL) {
            return 0;
        }
        if (out_size < needed) {
            return -1;
        }
        out[0] = (uint8_t)(short_base + len);
        return 0;
    }

    /* Long form: base + lenlen, then len in big-endian. */
    uint8_t tmp[sizeof(size_t)];
    size_t tmp_len = 0;

    {
        size_t v = len;
        /* Build big-endian representation without leading zeros. */
        for (int i = (int)(sizeof(size_t) - 1); i >= 0; --i) {
            tmp[i] = (uint8_t)(v & 0xFF);
            v >>= 8;
        }
        size_t i = 0;
        while (i < sizeof(size_t) && tmp[i] == 0) {
            ++i;
        }
        tmp_len = sizeof(size_t) - i;
        if (tmp_len == 0) {
            tmp_len = 1;
            tmp[sizeof(size_t) - 1] = 0;
            i = sizeof(size_t) - 1;
        }

        size_t needed = 1 + tmp_len;
        if (out_len) {
            *out_len = needed;
        }
        if (out == NULL) {
            return 0;
        }
        if (out_size < needed) {
            return -1;
        }

        out[0] = (uint8_t)(long_base + tmp_len);
        memcpy(out + 1, tmp + i, tmp_len);
    }

    return 0;
}

int web3c_rlp_encode_uint64(uint64_t value,
                            uint8_t *out,
                            size_t out_size,
                            size_t *out_len)
{
    /* Special case: zero -> empty string (0x80). */
    if (value == 0) {
        size_t needed = 1;
        if (out_len) {
            *out_len = needed;
        }
        if (out == NULL) {
            return 0;
        }
        if (out_size < needed) {
            return -1;
        }
        out[0] = 0x80;
        return 0;
    }

    /* Convert to big-endian bytes without leading zeros. */
    uint8_t buf[8];
    for (int i = 7; i >= 0; --i) {
        buf[i] = (uint8_t)(value & 0xFF);
        value >>= 8;
    }

    size_t offset = 0;
    while (offset < sizeof(buf) && buf[offset] == 0) {
        ++offset;
    }
    size_t len = sizeof(buf) - offset;

    /* If single byte <= 0x7f, encode as itself. */
    if (len == 1 && buf[offset] <= 0x7f) {
        size_t needed = 1;
        if (out_len) {
            *out_len = needed;
        }
        if (out == NULL) {
            return 0;
        }
        if (out_size < needed) {
            return -1;
        }
        out[0] = buf[offset];
        return 0;
    }

    /* Otherwise encode as a string. */
    size_t prefix_len = 0;
    int rc = rlp_write_length(len, 0x80, 0xb7, out, out_size, &prefix_len);
    if (rc != 0) {
        return rc;
    }

    size_t needed = prefix_len + len;
    if (out_len) {
        *out_len = needed;
    }
    if (out == NULL) {
        return 0;
    }
    if (out_size < needed) {
        return -1;
    }

    memcpy(out + prefix_len, buf + offset, len);
    return 0;
}

int web3c_rlp_encode_bytes(const uint8_t *data,
                           size_t len,
                           uint8_t *out,
                           size_t out_size,
                           size_t *out_len)
{
    /* Empty string. */
    if (len == 0) {
        size_t needed = 1;
        if (out_len) {
            *out_len = needed;
        }
        if (out == NULL) {
            return 0;
        }
        if (out_size < needed) {
            return -1;
        }
        out[0] = 0x80;
        return 0;
    }

    if (data == NULL) {
        return -1;
    }

    /* Single byte in [0x00, 0x7f] */
    if (len == 1 && data[0] <= 0x7f) {
        size_t needed = 1;
        if (out_len) {
            *out_len = needed;
        }
        if (out == NULL) {
            return 0;
        }
        if (out_size < needed) {
            return -1;
        }
        out[0] = data[0];
        return 0;
    }

    size_t prefix_len = 0;
    int rc = rlp_write_length(len, 0x80, 0xb7, out, out_size, &prefix_len);
    if (rc != 0) {
        return rc;
    }

    size_t needed = prefix_len + len;
    if (out_len) {
        *out_len = needed;
    }
    if (out == NULL) {
        return 0;
    }
    if (out_size < needed) {
        return -1;
    }

    memcpy(out + prefix_len, data, len);
    return 0;
}

int web3c_rlp_encode_list_header(size_t payload_len,
                                 uint8_t *out,
                                 size_t out_size,
                                 size_t *out_len)
{
    return rlp_write_length(payload_len, 0xC0, 0xF7,
                            out, out_size, out_len);
}