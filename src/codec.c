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

#include "codec.h"
#include "codecbase.h"
#include "base64.h"
#include "base32.h"
#include "base16.h"
#include "urlencoding.h"

typedef struct _codec {
    CODECode code;
    CODECWork work;
    CDCStream *result;
    
    CODECProtocol protocol;
    union {
        urlencoding url;
        base16 b16;
        base32 b32;
        base64 b64;
    }protocols;
}_codec;

void codec_reset(CODEC codec) {
	_codec *c = (_codec *)codec;
    if (!codec) {
        return;
    }
    
    c->code = CODECNeedSpecailProtocol;
    stream_clear(c->result);
    
    c->protocol = CODECProtocolNone;
    if (c->work.cleanup) {
        c->work.cleanup(&c->protocols);
    }
    memset(&c->work, 0, sizeof(CODECWork));
}

CODEC codec_init() {
    _codec *c = malloc(sizeof(_codec));
    
    c->result = stream_init(0);
    c->code = CODECNeedSpecailProtocol;
    
    c->protocol = CODECProtocolNone;
    memset(&c->protocols, 0, sizeof(c->protocols));
    
    memset(&c->work, 0, sizeof(CODECWork));
    
    return c;
}

void codec_cleanup(CODEC codec) {
	_codec *c = (_codec *)codec;
    if (!codec) {
        return;
    }
    
    stream_cleanup(c->result);
    
    if (c->work.cleanup) {
        c->work.cleanup(&c->protocols);
    }
    
    free(c);
}

#pragma mark - setopt
static CODECode _codec_special_protocol(_codec *c, long v) {
	CODECProtocol protocol;
	initfunc fn = 0;
    if (v < CODECProtocolNone || v >= CODECSupportCount) {
        return CODECInvalidInput;
    }
    
	protocol = (CODECProtocol)v;
    if (protocol == CODECProtocolNone) {
        return CODECNeedSpecailProtocol;
    }
    
    if (c->protocol == protocol) {
        return CODECOk;
    }
    
    if (c->protocol != CODECProtocolNone) {
        codec_reset(c);
    }
    
    switch (protocol) {
        case CODECBase64:
            fn = base64_init;
            break;
            
        case CODECBase32:
            fn = base32_init;
            break;
            
        case CODECBase16:
            fn = base16_init;
            break;
            
        case CODECURL:
            fn = urlencoding_init;
            break;
            
        default:
            break;
    }

    cdcassert(fn);
    fn(&c->protocols, &c->work);
    c->protocol = protocol;
    
    return CODECOk;
}

static CODECode _codec_setopt(_codec *c, CODECOption opt, va_list args) {
    if (opt == CODECSpecialProtocol) {
        return _codec_special_protocol(c, va_arg(args, long));
    }
    
    if (c->protocol == CODECProtocolNone) {
        return CODECNeedSpecailProtocol;
    }

    if (c->work.setup) {
        return c->work.setup(&c->protocols, opt, args);
    }
    
    return CODECIgnoredOption;
}

CODECode codec_setup(CODEC codec, CODECOption opt, ...) {
	_codec *c = (_codec *)codec;
	va_list args;
    if (!codec) {
        return CODECNullPtr;
    }
    
    va_start(args, opt);
    c->code = _codec_setopt(c, opt, args);
    va_end(args);
    
    return c->code;
}

static const CDCStream * _codec_work(_codec *c, CDCBOOL encoding, const byte *data, size_t datalen) {
	workfunc fn = 0;
    if (!c) {
        return 0;
    }
    
    if (c->protocol == CODECProtocolNone) {
        c->code = CODECNeedSpecailProtocol;
        return 0;
    }
    else {
        if (!data || !datalen) {
            c->code = CODECEmptyInput;
            return 0;
        }
        
        fn = encoding ? c->work.encoding : c->work.decoding;
        cdcassert(fn);
        if (fn) {
            stream_clear(c->result);
            c->code = fn(&c->protocols, data, datalen, c->result);
        }
        
        return c->code == CODECOk ? c->result : 0;
    }
}

const CDCStream * codec_encode(CODEC codec, const byte *data, size_t datalen) {
    return _codec_work(codec, CDCTRUE, data, datalen);
}

const CDCStream * codec_decode(CODEC codec, const byte *data, size_t datalen) {
    return _codec_work(codec, CDCFALSE, data, datalen);
}

CODECode codec_lasterror(CODEC codec) {
    if (!codec) {
        return CODECNullPtr;
    }
    
    return ((_codec *)codec)->code;
}