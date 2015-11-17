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

#ifndef base64_h
#define base64_h
#include "cdcdefs.h"

typedef enum {
    base64opt_urlsafe = 0,
    
    base64opt_chunk = 1 << 0,
    base64opt_padding = 1 << 1,
    base64opt_unsafechar = 1 << 2,
    
    base64opt_standard = base64opt_chunk | base64opt_padding | base64opt_unsafechar
}cdc_base64opt;

typedef struct cdc_base64 cdc_base64;

cdc_base64 * base64_init(cdc_base64opt opt);
void base64_cleanup(cdc_base64 *p);

size_t base64_encode_length(cdc_base64 *ins, size_t datalen);
void base64_encode(cdc_base64 *ins, const byte *data, size_t datalen, char *buf, size_t *buflen);

size_t base64_decode_length(size_t datalen);
BOOL base64_decode(const char *data, size_t datalen, byte *buf, size_t *buflen);

#endif /* base64_h */
