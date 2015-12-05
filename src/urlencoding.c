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

#include "urlencoding.h"
#include "cdcstream.h"

static void _urlencoding_cleanup(CODECBase *p) {
    struct urlencoding *ue = (struct urlencoding *)p;
    free(ue->safechr);
}

static void _url_encoding(const byte *safechr, CDCStream *st, const byte *data, size_t dataLen);
static BOOL _url_decoding(const byte *safechr, CDCStream *st, const byte *data, size_t dataLen);

static CODECode _urlencoding_work(CODECBase *p, const CDCStream *st);

void *urlencoding_init(CODECBase *p) {
    struct urlencoding *ue = (struct urlencoding *)p;
    ue->safechr = malloc(0xff * sizeof(byte));
    memset(ue->safechr, 0xff, 0xff * sizeof(byte));
    
    byte c = 0;
    for (c = 'A'; c <= 'Z'; ++c) {
        ue->safechr[c] = c;
    }
    
    for (c = 'a'; c <= 'z'; ++c) {
        ue->safechr[c] = c;
    }
    
    for (c = '0'; c <= '9'; ++c) {
        ue->safechr[c] = c;
    }
    
    ue->safechr['-'] = '-';
    ue->safechr['_'] = '_';
    ue->safechr['.'] = '.';
    ue->safechr['*'] = '*';
    ue->safechr[' '] = ' ';
    
    ue->cleanup = _urlencoding_cleanup;
    ue->work = _urlencoding_work;
    return p;
}

CODECode _urlencoding_work(CODECBase *p, const CDCStream *st) {
    struct urlencoding *ue = (struct urlencoding *)p;
    if (p->method == CODECEncoding) {
        _url_encoding(ue->safechr, p->result, stream_data(st), stream_size(st));
    }
    else {
        if (!_url_decoding(ue->safechr, p->result, stream_data(st), stream_size(st))) {
            return CODECInvalidInput;
        }
    }
    
    return CODECOk;
}

#pragma mark - encoding
static byte _url_digit(byte b) {
    cdcassert(b <= 0x0f);
    if (b < 10) {
        return b + '0';
    }
    else {
        return b - 10 + 'A';
    }
}
void _url_encoding(const byte *safechr, CDCStream *buf, const byte *data, size_t dataLen) {
    size_t i = 0;
    byte c = 0;
    byte escape[3] = {0};
    escape[0] = '%';
    for (i = 0; i < dataLen; ++i) {
        c = data[i];
        if (c == 0xff) {
            stream_write_bytes(buf, (const byte *)"%FF", 3);
        }
        else if(safechr[c] == 0xff) {
            escape[1] = _url_digit((c >> 4) & 0x0f);
            escape[2] = _url_digit(c & 0x0f);
            stream_write_bytes(buf, escape, 3);
        }
        else {
            stream_write_b(buf, c == ' ' ? '+' : c);
        }
    }
}

#pragma mark - decoding
static byte _url_de_digit(byte b) {
    if (b >= '0' && b <= '9') {
        return b - '0';
    }
    else if (b >= 'A' && b <= 'F') {
        return b - 'A' + 10;
    }
    else {
        return 0xff;
    }
}

static BOOL _url_de_escape(byte *escape) {
    cdcassert(escape);
    byte b0 = _url_de_digit(escape[0]);
    byte b1 = _url_de_digit(escape[1]);
    if (b0 > 0x0f || b1 > 0x0f) {
        return FALSE;
    }
    
    b1 |= ((b0 << 4) & 0xf0);
    escape[0] = b1;
    return TRUE;
}

BOOL _url_decoding(const byte *safechr, CDCStream *buf, const byte *data, size_t dataLen) {
    size_t i = 0;
    byte c = 0;
    byte escape[2] = {0};
    for (i = 0; i < dataLen;) {
        c = data[i];
        if (c == '%') {
            if (i + 2 >= dataLen) {
                return FALSE;
            }
            
            escape[0] = data[i + 1];
            escape[1] = data[i + 2];
            if (!_url_de_escape(escape)) {
                return FALSE;
            }
            
            stream_write_b(buf, escape[0]);
            i += 3;
        }
        else if (c == '+') {
            stream_write_b(buf, ' ');
            ++i;
        }
        else if (safechr[c] == 0xff) {
            return FALSE;
        }
        else {
            stream_write_b(buf, c);
            ++i;
        }
    }
    
    return TRUE;
}