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

static CODECode _base64_setopt(CODECBase *p, CODECOption opt, va_list args);
static CODECode _base64_work(CODECBase *p, const CODECData *data);

#pragma mark - init
void *base64_init(CODECBase *p) {
    struct base64 *ptr = (struct base64 *)p;
    ptr->setup = _base64_setopt;
    ptr->work = _base64_work;
    
    baseN_init(&ptr->bn, 3, 6, (char *)_standard_table, (byte *)_decode_table, 0x3f);
    return ptr;
}

CODECode _base64_setopt(CODECBase *p, CODECOption opt, va_list args) {
    long larg = 0;
    struct base64 *ptr = (struct base64 *)p;
    CODECode code = CODECOk;
    switch (opt) {
        case CODECBase64SafeChar:
            larg = va_arg(args, long);
            ptr->bn.entable = larg ? (char *)_urlsafe_table : (char *)_standard_table;
            break;
            
        case CODECBase64UrlSafe:
            larg = va_arg(args, long);
            ptr->bn.chunkled = !larg;
            ptr->bn.padding = !larg;
            ptr->bn.entable = larg ? (char *)_urlsafe_table : (char *)_standard_table;
            break;
            
        default:
            code = baseN_setup(&ptr->bn, opt, args);
            break;
    }
    
    return code;
}

CODECode _base64_work(CODECBase *p, const CODECData *data) {
    struct base64 *ptr = (struct base64 *)p;
    return baseN_work(&ptr->bn, p, data);
}