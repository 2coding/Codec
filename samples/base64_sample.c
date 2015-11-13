//
//  base64_sample.c
//  CodecSample
//
//  Created by hesy on 15/11/13.
//  Copyright © 2015年 2Coding. All rights reserved.
//

#include "base64_sample.h"
#include "base64.h"
#include <string.h>

void sample_encode_default() {
    const char *data = "hello world";
    size_t len = strlen(data);
    cdc_base64 *base64 = base64_init(base64opt_standard);
    
    size_t buflen = base64_encode_length(base64, len);
    char *buf = malloc(buflen + 1);
    base64_encode(base64, (const byte *)data, len, buf, &buflen);
    buf[buflen] = 0;
    assert(strcmp(buf, "aGVsbG8gd29ybGQ=") == 0);
    
    free(buf);
    base64_cleanup(base64);
}

void sample_encode_urlsafe() {
    const char *data = "hello world";
    size_t len = strlen(data);
    cdc_base64 *base64 = base64_init(base64opt_urlsafe);
    
    size_t buflen = base64_encode_length(base64, len);
    char *buf = malloc(buflen + 1);
    base64_encode(base64, (const byte *)data, len, buf, &buflen);
    buf[buflen] = 0;
    assert(strcmp(buf, "aGVsbG8gd29ybGQ") == 0);
    
    free(buf);
    base64_cleanup(base64);
}

void sample_decode() {
    const char *data = "aGVsbG8gd29ybGQ";
    size_t datalen = strlen(data);
    size_t buflen = base64_decode_length(datalen);
    byte *buf = malloc(buflen + 1);
    base64_decode(data, datalen, buf, &buflen);
    buf[buflen] = 0;
    assert(strcmp((const char *)buf, "hello world") == 0);
    
    free(buf);
}