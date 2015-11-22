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

#ifndef base64_hpp
#define base64_hpp

#include <gtest/gtest.h>
#include "codec.h"

static std::string _base64encode(const std::string &data, CODECOption opt, long v) {
    CODEC p = codec_init(CODECBase64, CODECEncoding);
    codec_setup(p, opt, v);
    CODECData cdata;
    cdata.data = (byte *)data.data();
    cdata.length = data.length();
    const CODECData *buf = codec_work(p, &cdata);
    if (!buf) {
        return "";
    }
    
    std::string ret((const char *)buf->data, buf->length);
    codec_cleanup(p);
    
    return ret;
}

static std::string _base64decode(const std::string &data, CODECode &code) {
    CODEC p = codec_init(CODECBase64, CODECDecoding);
    
    CODECData cdata;
    cdata.data = (byte *)data.data();
    cdata.length = data.length();
    
    const CODECData *buf = codec_work(p, &cdata);
    code = codec_lasterror(p);
    if (!buf) {
        return "";
    }
    
    std::string ret((const char *)buf->data, buf->length);
    codec_cleanup(p);
    
    return ret;
}

TEST(base64_tests, encode_option)
{
    std::string result = _base64encode("abcdefghijklmnopqrstuvwxyz-_0123456789/!?ABCDEFGHIJKLMNOPQRSTUVWXYZ", CODECBaseNChunkled, 0);
    EXPECT_EQ(result, "YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXotXzAxMjM0NTY3ODkvIT9BQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWg==");
    
    result = _base64encode("abcdefghijklmnopqrstuvwxyz-_0123456789/!?ABCDEFGHIJKLMNOPQRSTUVWXYZ", CODECBaseNPadding, 0);
    EXPECT_EQ(result, "YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXotXzAxMjM0NTY3ODkvIT9BQkNERUZHSElKS0xNTk9Q\r\nUVJTVFVWV1hZWg");
}

TEST(base64_tests, ignore_option)
{
    CODEC p  = codec_init(CODECBase64, CODECEncoding);
    codec_cleanup(p);
}

TEST(base64_tests, encode_standard)
{
    std::string result = _base64encode("hello world", CODECBase64UrlSafe, 0);
    EXPECT_EQ(result, "aGVsbG8gd29ybGQ=");
    
    result = _base64encode("abcdefghijklmnopqrstuvwxyz-_0123456789/!?ABCDEFGHIJKLMNOPQRSTUVWXYZ", CODECBase64UrlSafe, 0);
    EXPECT_EQ(result, "YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXotXzAxMjM0NTY3ODkvIT9BQkNERUZHSElKS0xNTk9Q\r\nUVJTVFVWV1hZWg==");
}

TEST(base64_tests, encode_urlsafe)
{
    std::string result = _base64encode("hello world", CODECBase64UrlSafe, 1L);
    EXPECT_EQ(result, "aGVsbG8gd29ybGQ");
    
    result = _base64encode("abcdefghijklmnopqrstuvwxyz-_0123456789/!?ABCDEFGHIJKLMNOPQRSTUVWXYZ", CODECBase64UrlSafe, 1L);
    EXPECT_EQ(result, "YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXotXzAxMjM0NTY3ODkvIT9BQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWg");
}

TEST(base64_tests, encode_empty)
{
    std::string result = _base64encode("", CODECBase64UrlSafe, 0L);
    EXPECT_EQ(result, "");
}

TEST(base64_tests, decode_standard)
{
    CODECode code = CODECOk;
    std::string result = _base64decode("aGVsbG8gd29ybGQ=", code);
    EXPECT_EQ(result, "hello world");
    EXPECT_EQ(code, CODECOk);
    
    result = _base64decode("YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXotXzAxMjM0NTY3ODkvIT9BQkNERUZHSElKS0xNTk9Q\r\nUVJTVFVWV1hZWg==", code);
    EXPECT_EQ(result, "abcdefghijklmnopqrstuvwxyz-_0123456789/!?ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    EXPECT_EQ(code, CODECOk);
}

TEST(base64_tests, decode_urlsafe)
{
    CODECode code = CODECOk;
    std::string result = _base64decode("aGVsbG8gd29ybGQ", code);
    EXPECT_EQ(result, "hello world");
    EXPECT_EQ(code, CODECOk);
    
    result = _base64decode("YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXotXzAxMjM0NTY3ODkvIT9BQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWg", code);
    EXPECT_EQ(result, "abcdefghijklmnopqrstuvwxyz-_0123456789/!?ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    EXPECT_EQ(code, CODECOk);
}

TEST(base64_tests, decode_empty)
{
    CODECode code = CODECOk;
    std::string result = _base64decode("", code);
    EXPECT_EQ(result, "");
    EXPECT_EQ(code, CODECEmptyInput);
}

TEST(base64_tests, decode_invalid_input)
{
    CODECode code = CODECOk;
    std::string result = _base64decode("aGV?sbG8gd29ybGQ", code);
    EXPECT_EQ(result, "");
    EXPECT_EQ(code, CODECInvalidInput);
}

TEST(base64_tests, nullptr_test)
{
    CODEC p = codec_init(CODECBase64, CODECEncoding);
    EXPECT_EQ(codec_work(p, 0), nullptr);
    EXPECT_EQ(codec_lasterror(p), CODECEmptyInput);
    codec_cleanup(p);
    
    p = codec_init(CODECBase64, CODECDecoding);
    EXPECT_EQ(codec_work(p, 0), nullptr);
    EXPECT_EQ(codec_lasterror(p), CODECEmptyInput);
    codec_cleanup(p);
    
    EXPECT_EQ(codec_work(0, 0), nullptr);
    EXPECT_EQ(codec_setup(0, CODECBase64UrlSafe, 1L), CODECNullPtr);
    EXPECT_EQ(codec_lasterror(0), CODECNullPtr);
}

#endif