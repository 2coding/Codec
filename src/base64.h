//
//  base64.h
//  Codec.Apple
//
//  Created by hesy on 15/11/11.
//  Copyright © 2015年 2Coding. All rights reserved.
//

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
