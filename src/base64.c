//
//  base64.c
//  Codec.Apple
//
//  Created by hesy on 15/11/11.
//  Copyright © 2015年 2Coding. All rights reserved.
//

#include "base64.h"

static const int _chunklen = 76;

struct cdc_base64 {
    cdc_base64opt opt;
};

cdc_base64 * base64_init(cdc_base64opt opt) {
    cdc_base64 *ins = malloc(sizeof(cdc_base64));
    ins->opt = opt;
    return ins;
}

void base64_cleanup(cdc_base64 *p) {
    free(p);
}

#pragma mark - encoding
static const char _standard_table[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G',
    'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U',
    'V', 'W', 'X', 'Y', 'Z', 'a', 'b',
    'c', 'd', 'e', 'f', 'g', 'h', 'i',
    'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v', 'w',
    'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+',
    '/'};

static const char _urlsafe_table[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G',
    'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U',
    'V', 'W', 'X', 'Y', 'Z', 'a', 'b',
    'c', 'd', 'e', 'f', 'g', 'h', 'i',
    'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v', 'w',
    'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '-',
    '_'};

size_t base64_encode_length(cdc_base64 *ins, size_t datalen) {
    size_t times = 0, len = 0;
    int mod = 0;
    
    cdcassert(ins);
    if (!datalen) {
        return 0;
    }
    
    times = datalen / 3;
    mod = datalen % 3;
    
    if (ins->opt & base64opt_padding) {
        mod = 4;
    }
    else {
        mod = mod ? mod + 1 : 0;
    }
    
    len = times * 4 + mod;
    if (ins->opt & base64opt_chunk) {
        len += (len  - 1) / _chunklen * 2;
    }
    
    return len;
}

static size_t _fill_chunk(cdc_base64 *ins, char *buf, size_t idx) {
    if ((ins->opt & base64opt_chunk)
        && idx > 0
        && (idx % _chunklen == 0)) {
        buf[idx++] = '\r';
        buf[idx++] = '\n';
    }
    
    return idx;
}

static size_t _encode_fillbuf(cdc_base64 *ins, const byte *data, char *buf, size_t idx) {
    idx = _fill_chunk(ins, buf, idx);
    
    uint32_t t = data[2] | ((data[1] << 8) & 0xff00) | ((data[0] << 16) & 0xff0000);
    
    const char *table = ins->opt & base64opt_unsafechar ? _standard_table : _urlsafe_table;
    buf[idx++] = table[(t >> 18) & 0x3f];
    buf[idx++] = table[(t >> 12) & 0x3f];
    buf[idx++] = table[(t >> 6) & 0x3f];
    buf[idx++] = table[t & 0x3f];
    
    return idx;
}

static size_t _fill_left_buf(cdc_base64 *ins,
                          const byte *data, size_t datalen,
                          char *buf, size_t idx) {
    idx = _fill_chunk(ins, buf, idx);
    
    int left = (int)(datalen % 3);
    if (!left) {
        return idx;
    }
    
    uint32_t t = 0;
    size_t i = datalen - left;
    t = (data[i++] << 8) & 0xff00;
    
    const char *table = ins->opt & base64opt_unsafechar ? _standard_table : _urlsafe_table;
    if (i >= datalen) {
        buf[idx++] = table[(t >> 10) & 0x3f];
        buf[idx++] = table[(t >> 4) & 0x3f];
        
        if (ins->opt & base64opt_padding) {
            buf[idx++] = '=';
            buf[idx++] = '=';
        }
    }
    else {
        t = (t << 8) & 0xff0000;
        t |= ((data[i] << 8) & 0xff00);
        buf[idx++] = table[(t >> 18) & 0x3f];
        buf[idx++] = table[(t >> 12) & 0x3f];
        buf[idx++] = table[(t >> 6) & 0x3f];
        
        if (ins->opt & base64opt_padding) {
            buf[idx++] = '=';
        }
    }
    
    return idx;
}

void base64_encode(cdc_base64 *ins,
                   const byte *data, size_t datalen,
                   char *buf, size_t *buflen) {
    cdcassert(ins);
    cdcassert(buf);
    cdcassert(buflen);
    if (!data || !datalen) {
        *buflen = 0;
        buf[0] = 0;
        return;
    }
    
    size_t i = 0, idx = 0, n = datalen / 3;
    for (i = 0; i < n; ++i) {
        idx = _encode_fillbuf(ins, data + i * 3, buf, idx);
    }
    
    *buflen = _fill_left_buf(ins, data, datalen, buf, idx);
}

#pragma mark - decoding
size_t base64_decode_length(size_t datalen) {
    if (!datalen) {
        return 0;
    }
    
    double len = datalen / 4.0;
    len = ceil(len);
    return len * 3;
}

static const byte _decode_table[] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0x3e, 0xff, 0x3e, 0xff, 0x3f, 0x34,
    0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b,
    0x3c, 0x3d, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
    0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12,
    0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
    0xff, 0xff, 0xff, 0xff, 0x3f, 0xff, 0x1a,
    0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21,
    0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0xff, 0xff, 0xff, 
    0xff
};
static size_t _decode_2chars(const char *data, byte *buf, size_t *idx) {
    byte b0 = _decode_table[data[0]],
    b1 = _decode_table[data[1]];
    
    if (b0 == 0xff
        || b1 == 0xff) {
        return 0;
    }
    
    uint32_t t = (b1 & 0x3f) | ((b0 & 0x3f) << 6);
    size_t i = *idx;
    //6 + 6 = 8 + 4
    buf[i++] = (t >> 4) & 0xff;
    *idx = i;
    
    return 2;
}

static size_t _decode_3chars(const char *data, byte *buf, size_t *idx) {
    byte b0 = _decode_table[data[0]],
    b1 = _decode_table[data[1]],
    b2 = _decode_table[data[2]];
    
    if (b0 == 0xff
        || b1 == 0xff
        || b2 == 0xff) {
        return 0;
    }
    
    uint32_t t = (b2 & 0x3f) | ((b1 & 0x3f) << 6) | ((b0 & 0x3f) << 12);
    size_t i = *idx;
    //6 + 6 + 6 = 8 + 8 + 2
    buf[i++] = (t >> 10) & 0xff;
    buf[i++] = (t >> 2) & 0xff;
    *idx = i;
    
    return 3;
}

static size_t _decode_4chars(const char *data, byte *buf, size_t *idx) {
    if (data[2] == '=') {
        return _decode_2chars(data, buf, idx) ? 4 : 0;
    }
    
    if (data[3] == '=') {
        return _decode_3chars(data, buf, idx) ? 4 : 0;
    }
    
    byte b0 = _decode_table[data[0]],
        b1 = _decode_table[data[1]],
        b2 = _decode_table[data[2]],
        b3 = _decode_table[data[3]];
    
    if (b0 == 0xff
        || b1 == 0xff
        || b2 == 0xff
        || b3 == 0xff) {
        return 0;
    }
    
    uint32_t t = (b3 & 0x3f) | ((b2 & 0x3f) << 6) | ((b1 & 0x3f) << 12) | ((b0 & 0x3f) << 18);
    size_t i = *idx;
    buf[i++] = (t >> 16) & 0xff;
    buf[i++] = (t >> 8) & 0xff;
    buf[i++] = t & 0xff;
    *idx = i;
    
    return 4;
}

BOOL base64_decode(const char *data, size_t datalen, byte *buf, size_t *buflen) {
    cdcassert(buflen);
    cdcassert(buf);
    if (!data || !datalen) {
        buf[0] = 0;
        *buflen = 0;
        return TRUE;
    }
    
    size_t i = 0, idx = 0, n = 0;
    while (i < datalen) {
        if (data[i] == '\r') {
            i += 2;
        }
        
        n = 0;
        if (i + 3 < datalen) {
            if (!(n = _decode_4chars(data + i, buf, &idx))) {
                return FALSE;
            }
        }
        else if (i + 2 < datalen) {
            if (!(n = _decode_3chars(data + i, buf, &idx))) {
                return FALSE;
            }
        }
        else if (i + 1 < datalen) {
            if (!(n = _decode_2chars(data + i, buf, &idx))) {
                return FALSE;
            }
        }
        
        i += n;
    }
    
    *buflen = idx;
    return TRUE;
}