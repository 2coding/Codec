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

#include "cdcstream.h"

#define _max(a, b) ((a) > (b) ? (a) : (b))
#define _min(a, b) ((a) < (b) ? (a) : (b))

struct CDCStream {
    byte *data;
    size_t size;
    size_t length;
};

CDCStream *stream_init(size_t len) {
    return stream_init_data(0, len);
}

CDCStream *stream_init_data(const byte *data, size_t len) {
    CDCStream *st = malloc(sizeof(CDCStream));
    
    size_t size = data ? len : 0;
    size_t length = size ? size * 2 : len;
    st->length = _max(length, 0x0f);
    st->data = malloc(st->length * sizeof(byte));
    if (!st->data) {
        free(st);
        return 0;
    }
    
    st->size = size;
    if (data) {
        memcpy(st->data, data, st->size * sizeof(byte));
    }
    
    return st;
}

void stream_cleanup(CDCStream *st) {
    if (!st) {
        return;
    }
    
    free(st->data);
    free(st);
}

size_t stream_size(const CDCStream *st) {
    cdcassert(st);
    if (!st) {
        return 0;
    }
    
    return st->size;
}

const byte *stream_data(const CDCStream *st) {
    cdcassert(st);
    if (!st) {
        return 0;
    }
    
    return st->data;
}

CDCBOOL stream_empty(const CDCStream *st) {
    return !st || !st->size;
}

#pragma mark - write
static CDCBOOL _check_stream(CDCStream *st, size_t len) {
    cdcassert(len);
    if (!st || !st->data || !len) {
        return CDCFALSE;
    }
    
    size_t newLen = st->size + len;
    if (newLen > st->length) {
        newLen = _max(newLen, st->length * 2);
        byte *data = malloc(newLen * sizeof(byte));
        if (!data) {
            return CDCFALSE;
        }
        
        memcpy(data, st->data, st->size * sizeof(byte));
        free(st->data);
        st->data = data;
        st->length = newLen;
    }
    
    return CDCTRUE;
}

size_t stream_write_b(CDCStream *st, byte b) {
    byte data[] = {b};
    return stream_write_bytes(st, data, 1);
}

size_t stream_write_bytes(CDCStream *st, const byte *data, size_t dataLen) {
    cdcassert(st);
    if (!st) {
        return 0;
    }
    
    if (!data || !dataLen) {
        return st->size;
    }
    
    if (!_check_stream(st, dataLen)) {
        return 0;
    }
    
    memcpy(st->data + st->size, data, dataLen * sizeof(byte));
    st->size += dataLen;
    return st->size;
}

#pragma mark - read
static void _resize_stream(CDCStream *st) {
    if (st->size > st->length / 4 || st->length <= 0x0f) {
        return;
    }
    
    size_t len = _max(st->length / 2, 0x0f);
    byte *data = malloc(len * sizeof(byte));
    if (!data) {
        return;
    }
    
    if (st->size) {
        memcpy(data, st->data, st->size * sizeof(byte));
    }
    free(st->data);
    st->data = data;
    st->length = len;
}

size_t stream_read(CDCStream *st, byte *buf, size_t *readLen) {
    cdcassert(st);
    if (!st) {
        return 0;
    }
    
    if (!buf || !readLen) {
        return 0;
    }
    
    size_t len = _min(st->size, *readLen);
    memcpy(buf, st->data, len * sizeof(byte));
    st->size -= len;
    
    _resize_stream(st);
    *readLen = len;
    return st->size;
}

void stream_clear(CDCStream *st) {
    if (!st->size) {
        return;
    }
    
    st->size = 0;
    _resize_stream(st);
}
