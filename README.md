Codec
==========
General encoding/decoding algorithms (for example base64, URL)

### Support Algorithms
    base64
    base32
    base16
    url codec

### Requirement
	Xcode 7.1.1+
	IOS 6.0+
	OSX 10.4+
    VISUAL C++ 2010
    
### Example:
    CODEC p = codec_init();
    codec_setup(p, CODECSpecialProtocol, CODECBase64);
    const CDCStream *buf = codec_encode(p, (const byte *)"hello world", 11);
    codec_cleanup(p);

### See Also
[base64/base32/base16 encodings standard](http://www.ietf.org/rfc/rfc4648.txt)<br>
[application/x-www-form-urlencoded](http://www.w3.org/TR/html4/interact/forms.html#h-17.13.4.1) 
