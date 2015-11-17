/**
 Copyright (c) 2015-present, 2coding, Inc.
 All rights reserved.
 
 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
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