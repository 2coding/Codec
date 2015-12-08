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
#ifndef test_base_hpp
#define test_base_hpp
extern "C" {
#include "codec.h"
}

CODEC _c;
static void test_init() {
    _c = codec_init();
}

static void test_cleanup() {
    codec_cleanup(_c);
}

static std::string _test_encoding(const std::string &data, CODECProtocol protocol, CODECOption opt, long v, CODECode &code) {
    codec_reset(_c);
    codec_setup(_c, CODECSpecialProtocol, protocol);
    codec_setup(_c, opt, v);
    const CDCStream *buf = codec_encode(_c, (const byte *)data.data(), data.length());
    code = codec_lasterror(_c);
    if (stream_empty(buf)) {
        return "";
    }
    
    std::string ret((const char *)stream_data(buf), stream_size(buf));
    return ret;
}

static std::string _test_decoding(const std::string &data, CODECProtocol protocol, CODECOption opt, long v, CODECode &code) {
    codec_reset(_c);
    codec_setup(_c, CODECSpecialProtocol, protocol);
    codec_setup(_c, opt, v);
    
    const CDCStream *buf = codec_decode(_c, (const byte *)data.data(), data.length());
    code = codec_lasterror(_c);
    if (stream_empty(buf)) {
        return "";
    }
    
    std::string ret((const char *)stream_data(buf), stream_size(buf));
    return ret;
}

#endif