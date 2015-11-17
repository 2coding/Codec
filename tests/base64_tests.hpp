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

static std::string _base64encode(const std::string &data, cdc_base64opt opt) {
    cdc_base64 *base64 = base64_init(opt);
    size_t buflen = base64_encode_length(base64, data.size());
    char *buf = new char[buflen + 1];
    base64_encode(base64, (unsigned char *)data.data(), data.size(), buf, &buflen);
    
    std::string ret(buf, buflen);
    delete [] buf;
    base64_cleanup(base64);
    
    return ret;
}

static std::string _base64decode(const std::string &data) {
    size_t buflen = base64_decode_length(data.size());
    byte *buf = new byte[buflen];
    if (!base64_decode(data.data(), data.size(), buf, &buflen)) {
        return "";
    }
    
    std::string ret((const char *)buf, buflen);
    delete [] buf;
    return ret;
}

TEST(base64_tests, encode_standard)
{
    std::string result = _base64encode("hello world", base64opt_standard);
    EXPECT_EQ(result, "aGVsbG8gd29ybGQ=");
    
    result = _base64encode("abcdefghijklmnopqrstuvwxyz-_0123456789/!?ABCDEFGHIJKLMNOPQRSTUVWXYZ", base64opt_standard);
    EXPECT_EQ(result, "YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXotXzAxMjM0NTY3ODkvIT9BQkNERUZHSElKS0xNTk9Q\r\nUVJTVFVWV1hZWg==");
}

TEST(base64_tests, encode_urlsafe)
{
    std::string result = _base64encode("hello world", base64opt_urlsafe);
    EXPECT_EQ(result, "aGVsbG8gd29ybGQ");
    
    result = _base64encode("abcdefghijklmnopqrstuvwxyz-_0123456789/!?ABCDEFGHIJKLMNOPQRSTUVWXYZ", base64opt_urlsafe);
    EXPECT_EQ(result, "YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXotXzAxMjM0NTY3ODkvIT9BQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWg");
}

TEST(base64_tests, encode_empty)
{
    std::string result = _base64encode("", base64opt_standard);
    EXPECT_EQ(result, "");
}

TEST(base64_tests, decode_standard)
{
    std::string result = _base64decode("aGVsbG8gd29ybGQ=");
    EXPECT_EQ(result, "hello world");
    
    result = _base64decode("YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXotXzAxMjM0NTY3ODkvIT9BQkNERUZHSElKS0xNTk9Q\r\nUVJTVFVWV1hZWg==");
    EXPECT_EQ(result, "abcdefghijklmnopqrstuvwxyz-_0123456789/!?ABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

TEST(base64_tests, decode_urlsafe)
{
    std::string result = _base64decode("aGVsbG8gd29ybGQ");
    EXPECT_EQ(result, "hello world");
    
    result = _base64decode("YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXotXzAxMjM0NTY3ODkvIT9BQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWg");
    EXPECT_EQ(result, "abcdefghijklmnopqrstuvwxyz-_0123456789/!?ABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

TEST(base64_tests, decode_empty)
{
    std::string result = _base64decode("");
    EXPECT_EQ(result, "");
}

#endif