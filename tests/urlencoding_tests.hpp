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
#ifndef urlencoding_tests_hpp
#define urlencoding_tests_hpp
#include <gtest/gtest.h>
#include "test_base.hpp"

static std::string _urlencode(const std::string &data) {
    CODECode code;
    return _test_encoding(data, CODECURL, CODECStandard, 1L, code);
}

static std::string _urldecode(const std::string &data) {
    CODECode code;
    return _test_decoding(data, CODECURL, CODECStandard, 1L, code);
}

TEST(urlcodec_tests, encode_empty)
{
    std::string result = _urlencode("");
    EXPECT_EQ(result, "");
}

TEST(urlcodec_tests, encode_standard)
{
    std::string result = _urlencode("你好世界+hello world");
    EXPECT_EQ(result, "%E4%BD%A0%E5%A5%BD%E4%B8%96%E7%95%8C%2Bhello+world");
}


TEST(urlcodec_tests, decode_standard)
{
    std::string result = _urldecode("%E4%BD%A0%E5%A5%BD%E4%B8%96%E7%95%8C%2Bhello+world");
    EXPECT_EQ(result, "你好世界+hello world");
}
#endif