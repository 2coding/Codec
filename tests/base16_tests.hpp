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

#ifndef base16_tests_hpp
#define base16_tests_hpp
#include <gtest/gtest.h>
#include "test_base.hpp"

static std::string _base16encode(const std::string &data) {
    return _test_encoding(data, CODECBase16, CODECEncoding, CODECStandard, 1L);
}

static std::string _base16decode(const std::string &data) {
    CODECode code;
    return _test_decoding(data, CODECBase16, CODECDecoding, CODECStandard, 1L, code);
}

static std::string _base16decode_ignorecase(const std::string &data) {
    CODECode code;
    return _test_decoding(data, CODECBase16, CODECDecoding, CODECBase16IgnoreCase, 1L, code);
}

TEST(base16_tests, encode_empty)
{
    std::string result = _base16encode("");
    EXPECT_EQ(result, "");
}

TEST(base16_tests, encode_standard)
{
    std::string result = _base16encode("foobar");
    EXPECT_EQ(result, "666F6F626172");
}

TEST(base16_tests, decode_standard)
{
    std::string result = _base16decode("666F6F626172");
    EXPECT_EQ(result, "foobar");
    
    result = _base16decode("666F\r\n6F626172");
    EXPECT_EQ(result, "foobar");
}

TEST(base16_tests, decode_ignorecase)
{
    std::string result = _base16decode_ignorecase("666f6F626172");
    EXPECT_EQ(result, "foobar");
}
#endif