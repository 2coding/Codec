//
//  main.c
//  CodecSample
//
//  Created by hesy on 15/11/13.
//  Copyright © 2015年 2Coding. All rights reserved.
//

#include <stdio.h>
#include "base64_sample.h"

int main(int argc, const char * argv[]) {
    sample_encode_default();
    sample_encode_urlsafe();
    
    sample_decode();
    return 0;
}
