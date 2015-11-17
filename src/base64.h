/**
 Copyright (c) 2015-present, 2coding, Inc.
 All rights reserved.
 
 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
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
