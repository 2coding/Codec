/**
 Copyright © 2015 2coding. All Rights Reserved.
 
 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 
 3. The name of the author may not be used to endorse or promote products derived
 from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 */

#include "base64.h"

static const int _chunklen = 76;

static size_t _base64_encode_length(struct base64 *p, size_t datalen);
static void _base64_encode(struct base64 *p,
                           const byte *data, size_t datalen,
                           byte *buf, size_t *buflen);
static size_t _base64_decode_length(size_t datalen);
static BOOL _base64_decode(struct base64* p, const byte *data, size_t datalen, byte *buf, size_t *buflen);

static CODECode _base64_setopt(CODECBase *p, CODECOption opt, va_list args);
static CODECode _base64_work(CODECBase *p, const CODECData *data);

#pragma mark - init
void *base64_init(CODECBase *p) {
    struct base64 *ptr = (struct base64 *)p;
    ptr->opt = base64opt_standard;
    ptr->setup = _base64_setopt;
    ptr->work = _base64_work;
    return ptr;
}

CODECode _base64_setopt(CODECBase *p, CODECOption opt, va_list args) {
    long larg = 0;
    struct base64 *ptr = (struct base64 *)p;
    CODECode code = CODECOk;
    switch (opt) {
        case CODECBaseNChunkled:
            larg = va_arg(args, long);
            if (larg) {
                ptr->opt |= base64opt_chunk;
            }
            else {
                ptr->opt &= ~base64opt_chunk;
            }
            break;
            
        case CODECBaseNPadding:
            larg = va_arg(args, long);
            if (larg) {
                ptr->opt |= base64opt_padding;
            }
            else {
                ptr->opt &= ~base64opt_padding;
            }
            break;
            
        case CODECBase64SafeChar:
            larg = va_arg(args, long);
            if (larg) {
                ptr->opt &= ~base64opt_unsafechar;
            }
            else {
                ptr->opt |= base64opt_unsafechar;
            }
            break;
            
        case CODECBase64UrlSafe:
            larg = va_arg(args, long);
            if (larg) {
                ptr->opt = base64opt_urlsafe;
            }
            else {
                ptr->opt = base64opt_standard;
            }
            break;
            
        default:
            code = CODECIgnoredOption;
            break;
    }
    
    return code;
}

CODECode _base64_work(CODECBase *p, const CODECData *data) {
    struct base64 *ptr = (struct base64 *)p;
    if (ptr->method == CODECEncoding) {
        size_t buflen = _base64_encode_length(ptr, data->length);
        if (!buflen) {
            return CODECEmptyInput;
        }
        
        CODECDATA_REINIT(ptr, buflen);
        _base64_encode(ptr, data->data, data->length, ptr->result->data, &ptr->result->length);
    }
    else {
        size_t buflen = _base64_decode_length(data->length);
        if (!buflen) {
            return CODECEmptyInput;
        }
        
        CODECDATA_REINIT(ptr, buflen);
        if (!_base64_decode(ptr, data->data, data->length, ptr->result->data, &ptr->result->length)) {
            return CODECInvalidInput;
        }
    }
    
    return CODECOk;
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

size_t _base64_encode_length(struct base64 *p, size_t datalen) {
    size_t times = 0, len = 0;
    int mod = 0;
    
    if (!datalen) {
        return 0;
    }
    
    times = datalen / 3;
    mod = datalen % 3;
    
    if (p->opt & base64opt_padding) {
        mod = 4;
    }
    else {
        mod = mod ? mod + 1 : 0;
    }
    
    len = times * 4 + mod;
    if (p->opt & base64opt_chunk) {
        len += (len  - 1) / _chunklen * 2;
    }
    
    return len;
}

static size_t _fill_chunk(struct base64 *p, byte *buf, size_t idx) {
    if ((p->opt & base64opt_chunk)
        && idx > 0
        && (idx % _chunklen == 0)) {
        buf[idx++] = '\r';
        buf[idx++] = '\n';
    }
    
    return idx;
}

static size_t _encode_fillbuf(struct base64 *p, const byte *data, byte *buf, size_t idx) {
    idx = _fill_chunk(p, buf, idx);
    
    uint32_t t = data[2] | ((data[1] << 8) & 0xff00) | ((data[0] << 16) & 0xff0000);
    
    const char *table = p->opt & base64opt_unsafechar ? _standard_table : _urlsafe_table;
    buf[idx++] = table[(t >> 18) & 0x3f];
    buf[idx++] = table[(t >> 12) & 0x3f];
    buf[idx++] = table[(t >> 6) & 0x3f];
    buf[idx++] = table[t & 0x3f];
    
    return idx;
}

static size_t _fill_left_buf(struct base64 *p,
                          const byte *data, size_t datalen,
                          byte *buf, size_t idx) {
    idx = _fill_chunk(p, buf, idx);
    
    int left = (int)(datalen % 3);
    if (!left) {
        return idx;
    }
    
    uint32_t t = 0;
    size_t i = datalen - left;
    t = (data[i++] << 8) & 0xff00;
    
    const char *table = p->opt & base64opt_unsafechar ? _standard_table : _urlsafe_table;
    if (i >= datalen) {
        buf[idx++] = table[(t >> 10) & 0x3f];
        buf[idx++] = table[(t >> 4) & 0x3f];
        
        if (p->opt & base64opt_padding) {
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
        
        if (p->opt & base64opt_padding) {
            buf[idx++] = '=';
        }
    }
    
    return idx;
}

void _base64_encode(struct base64 *p,
                   const byte *data, size_t datalen,
                   byte *buf, size_t *buflen) {
    cdcassert(p);
    cdcassert(buf);
    cdcassert(buflen);
    if (!data || !datalen) {
        *buflen = 0;
        buf[0] = 0;
        return;
    }
    
    size_t i = 0, idx = 0, n = datalen / 3;
    for (i = 0; i < n; ++i) {
        idx = _encode_fillbuf(p, data + i * 3, buf, idx);
    }
    
    *buflen = _fill_left_buf(p, data, datalen, buf, idx);
}

#pragma mark - decoding
size_t _base64_decode_length(size_t datalen) {
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
static size_t _decode_2chars(const byte *data, byte *buf, size_t *idx) {
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

static size_t _decode_3chars(const byte *data, byte *buf, size_t *idx) {
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

static size_t _decode_4chars(const byte *data, byte *buf, size_t *idx) {
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

BOOL _base64_decode(struct base64* p,
                    const byte *data, size_t datalen,
                    byte *buf, size_t *buflen) {
    cdcassert(p);
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