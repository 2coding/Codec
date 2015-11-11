//
//  base64.c
//  Codec.Apple
//
//  Created by hesy on 15/11/11.
//  Copyright © 2015年 2Coding. All rights reserved.
//

#include "base64.h"

static const int _chunklen = 76;
static const char _table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G',
    'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U',
    'V', 'W', 'X', 'Y', 'Z', 'a', 'b',
    'c', 'd', 'e', 'f', 'g', 'h', 'i',
    'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v', 'w',
    'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+',
    '-'};

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
uint64_t base64_encode_length(cdc_base64 *ins, uint64_t datalen) {
    uint64_t times = 0, len = 0;
    int mod = 0;
    
    cdcassert(ins);
    if (!datalen) {
        return 0;
    }
    
    times = datalen / 3;
    mod = datalen % 3;
    
    if (ins->opt | base64opt_padding) {
        mod = 4;
    }
    else {
        mod = mod ? mod + 1 : 0;
    }
    
    len = times * 4 + mod;
    if (ins->opt | base64opt_chunk) {
        len += (len  - 1) / _chunklen * 2;
    }
    
    return len;
}

static uint64_t _fill_chunk(cdc_base64 *ins, char *buf, uint64_t idx) {
    if ((ins->opt | base64opt_chunk)
        && idx > 0
        && (idx % _chunklen == 0)) {
        buf[idx++] = '\r';
        buf[idx++] = '\n';
    }
    
    return idx;
}

//static byte _reverse_byte(const byte b) {
//    static const byte _reverse_table[] = {0x0, 0x8, 0x4, 0xC, 0x2, 0xA, 0x6, 0xE, 0x1, 0x9, 0x5, 0xD, 0x3, 0xB, 0x7, 0xF};
//    
//    byte c = 0;
//    c |= (_reverse_table[b & 0xf]) << 4;
//    c |= _reverse_table[(b >> 4) & 0xf];
//    return c;
//}

static uint64_t _fillbuf(cdc_base64 *ins, const byte *data, char *buf, uint64_t idx) {
    idx = _fill_chunk(ins, buf, idx);
    
    uint32_t t = data[2] | ((data[1] << 8) & 0xff00) | ((data[0] << 16) & 0xff0000);
    
    buf[idx++] = _table[(t >> 18) & 0x3f];
    buf[idx++] = _table[(t >> 12) & 0x3f];
    buf[idx++] = _table[(t >> 6) & 0x3f];
    buf[idx++] = _table[t & 0x3f];
    
    return idx;
}

static uint64_t _fill_left_buf(cdc_base64 *ins,
                          const byte *data, uint64_t datalen,
                          char *buf, uint64_t idx) {
    idx = _fill_chunk(ins, buf, idx);
    
    int left = (int)(datalen % 3);
    if (!left) {
        return idx;
    }
    
    uint32_t t = 0;
    uint64_t i = datalen - left;
    t = (data[i++] << 8) & 0xff00;
    
    if (i >= datalen) {
        buf[idx++] = _table[(t >> 10) & 0x3f];
        buf[idx++] = _table[(t >> 4) & 0x3f];
    }
    else {
        t = (t << 8) & 0xff0000;
        t |= ((data[i] << 8) & 0xff00);
        buf[idx++] = _table[(t >> 18) & 0x3f];
        buf[idx++] = _table[(t >> 12) & 0x3f];
        buf[idx++] = _table[(t >> 6) & 0x3f];
    }
    
    if (ins->opt | base64opt_padding) {
        while (idx % 4 != 0) {
            buf[idx++] = '=';
        }
    }
    
    return idx;
}

void base64_encode(cdc_base64 *ins,
                   const byte *data, uint64_t datalen,
                   char *buf, uint64_t *buflen) {
    cdcassert(ins);
    cdcassert(buf);
    cdcassert(buflen);
    if (!data || !datalen) {
        *buflen = 0;
        buf[0] = 0;
        return;
    }
    
    uint64_t i = 0, idx = 0, n = datalen / 3;
    for (i = 0; i < n; ++i) {
        idx = _fillbuf(ins, data + i * 3, buf, idx);
    }
    
    *buflen = _fill_left_buf(ins, data, datalen, buf, idx);
}