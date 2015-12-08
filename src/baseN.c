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

#include "baseN.h"

static const int _chunklen = 76;

void baseN_init(baseN *p, byte group, byte bitslen, char *entable, byte *detable, byte maxchr, size_t mask) {
    cdcassert(p);
    cdcassert(entable);
    cdcassert(detable);
    cdcassert(group > 0);
    cdcassert(bitslen > 0 && bitslen < 8);
    cdcassert(mask > 0);
    
    p->group = group;
    p->bitslen = bitslen;
    p->egroup = group * 8 / bitslen;
    p->chunkled = CDCTRUE;
    p->padding = CDCTRUE;
    p->entable = entable;
    p->detable = detable;
    p->maxchr = maxchr;
    p->mask = mask;
}

CODECode baseN_setup(baseN *bn, CODECOption opt, va_list args) {
    CODECode code = CODECOk;
    switch (opt) {
        case CODECBaseNChunkled:
            bn->chunkled = va_arg(args, long);
            break;
            
        case CODECBaseNPadding:
            bn->padding = va_arg(args, long);
            break;
            
        default:
            code = CODECIgnoredOption;
            break;
    }
    
    return code;
}

#pragma mark - encoding
static void _chunk(const baseN *p, CDCStream *buf) {
    size_t size = stream_size(buf);
    if (p->chunkled
        && size > 0
        && size % _chunklen == 0) {
        stream_write_bytes(buf, (const byte *)"\r\n", 2);
    }
}

static void _encoding_group(const baseN *p, const byte *data, CDCStream *buf, byte group) {
    size_t i = 0;
    uint64_t t = 0;
    byte *tmp = (byte *)&t;
    for (i = 0; i < group; ++i) {
        tmp[i + 1] = data[group - i - 1];
    }
    
    int bits = (group + 1) * 8;
    const char *table = p->entable;
    uint64_t mask = p->mask;
    while (bits > 8) {
        bits -= p->bitslen;
        
        _chunk(p, buf);
        stream_write_b(buf, table[(t >> bits) & mask]);
    }
}

static void _encoding_left(const baseN *p, const byte *data, size_t datalen, CDCStream *buf) {
    size_t left = datalen % p->group;
    if (!left) {
        return ;
    }
    
    _chunk(p, buf);
    _encoding_group(p, data + (datalen - left), buf, left);
    
    float t = left * 8.0f / p->bitslen;
    t = ceilf(t);
    int padding = p->egroup - t;
    while (p->padding && padding > 0) {
        _chunk(p, buf);
        stream_write_b(buf, '=');
        --padding;
    }
}

CODECode baseN_encoding(void *p, const byte *data, size_t datalen, CDCStream *buf) {
    cdcassert(p);
    cdcassert(data && datalen);
    cdcassert(buf);
    
    baseN *bn = p;
    size_t i = 0, n = datalen / bn->group;
    for (i = 0; i < n; ++i) {
        _encoding_group(p, data + i * bn->group, buf, bn->group);
    }
    
    _encoding_left(p, data, datalen, buf);
    return CODECOk;
}

#pragma mark - decoding
static long _check_char(const baseN *p, byte c) {
    if (c == '\r' || c == '\n' || c == '=') {
        return 0;
    }
    
    if (c > p->maxchr || p->detable[c] == 0xff) {
        return -1;
    }
    
    return 1;
}

static void _decoding_group(const baseN *p, uint64_t tmp, size_t group, CDCStream *buf) {
    size_t k = 0, bitlens = 8 * (p->group - 1);
    for (k = 0; k < group; ++k) {
        stream_write_b(buf, (tmp >>  (bitlens -  8 * k)) & 0xff);
    }
}

CODECode baseN_decoding(void *p, const byte *data, size_t datalen, CDCStream *buf) {
    cdcassert(p);
    cdcassert(data && datalen);
    cdcassert(buf);
    
    baseN *bn = p;
    size_t i = 0, k = 0;
    uint64_t t = 0;
    long ret = 0;
    const byte *table = bn->detable;
    uint64_t mask = bn->mask;
    const byte bitlens = bn->bitslen * (bn->egroup - 1);
    byte c = 0;
    for (i = 0; i < datalen; ++i) {
        c = data[i];
        ret = _check_char(p, c);
        if (ret == 0) {
            continue;
        }
        else if (ret < 0) {
            return CODECInvalidInput;
        }
        
        t |= (table[c] & mask) << (bitlens - bn->bitslen * k);
        ++k;
        if (k == bn->egroup) {
            _decoding_group(p, t, bn->group, buf);
            k = 0;
            t = 0;
        }
    }
    
    if (k > 0 && t != 0) {
        _decoding_group(p, t, k * bn->bitslen / 8, buf);
    }
    
    return CODECOk;
}