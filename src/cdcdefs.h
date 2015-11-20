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

#ifndef cdcdefs_h
#define cdcdefs_h

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

typedef long int BOOL;
#define TRUE 1
#define FALSE 0

typedef uint8_t byte;

#define cdcassert assert

typedef enum {
    CODECBase64
}CODECProtocol;

typedef enum {
    CODECEncoding,
    CODECDecoding,
}CODECMethod;

typedef enum {
    CODECOk,
    
    CODECEmptyInput,
    CODECInvalidInput,
    
    CODECNullPtr
}CODECode;

typedef enum {
    CODECBaseNChunkled,
    CODECBaseNPadding,
    
    CODECBase64SafeChar,
    CODECBase64UrlSafe
    
}CODECOption;

typedef struct {
    byte *data;
    size_t length;
}CODECData;

#define CODECDATA_CLEANUP(pd) {if(pd) {free(pd->data); free(pd);}}

#endif /* cdcdefs_h */
