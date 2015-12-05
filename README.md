Codec
==========
General encoding/decoding algorithms (for example base64, URL)

### Support Algorithms
    base64
    base32
    base16
    url codec
    
### example:
    CODEC p = codec_init(CODECBase64, CODECEncoding);
    CDCStream *st = stream_init_data((const byte *)"hello world", 11);
    const CDCStream *buf = codec_work(p, st);
    stream_cleanup(st);
    codec_cleanup(p);

### see also
[base64/base32/base16 encodings standard](http://www.ietf.org/rfc/rfc4648.txt)/<br>
 [application/x-www-form-urlencoded](http://www.w3.org/TR/html4/interact/forms.html#h-17.13.4.1) 
