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

#ifndef base32_tests_hpp
#define base32_tests_hpp

#include <gtest/gtest.h>
#include "test_base.hpp"

static std::string _base32encode(const std::string &data, CODECOption opt, long v) {
    return _test_encoding(data, CODECBase32, CODECEncoding, opt, v);
}

static std::string _base32decodehex(const std::string &data, CODECode &code, long v) {
    return _test_decoding(data, CODECBase32, CODECDecoding, CODECBase32Hex, v, code);
}

static std::string _base32decode_ignorecase(const std::string &data, CODECode &code) {
    return _test_decoding(data, CODECBase32, CODECDecoding, CODECBase32IgnoreCase, 1L, code);
}

static std::string _base32decode(const std::string &data, CODECode &code) {
    return _base32decodehex(data, code, 0);
}

TEST(base32_tests, encode_empty)
{
    std::string result = _base32encode("", CODECBase32Hex, 0);
    EXPECT_EQ(result, "");
}

TEST(base32_tests, encode_standard)
{
    std::string result = _base32encode("hello world", CODECBase32Hex, 0);
    EXPECT_EQ(result, "NBSWY3DPEB3W64TMMQ======");
    
    result = _base32encode("abcdefghijklmnopqrstuvwxyz-_0123456789/!?ABCDEFGHIJKLMNOPQRSTUVWXYZ", CODECBase32Hex, 0);
    EXPECT_EQ(result, "MFRGGZDFMZTWQ2LKNNWG23TPOBYXE43UOV3HO6DZPIWV6MBRGIZTINJWG44DSLZBH5AUEQ2EIVDE\r\nOSCJJJFUYTKOJ5IFCUSTKRKVMV2YLFNA====");
}

TEST(base32_tests, encode_hex)
{
    std::string result = _base32encode("hello world", CODECBase32Hex, 1L);
    EXPECT_EQ(result, "D1IMOR3F41RMUSJCCG======");
    
    result = _base32encode("abcdefghijklmnopqrstuvwxyz-_0123456789/!?ABCDEFGHIJKLMNOPQRSTUVWXYZ", CODECBase32Hex, 1L);
    EXPECT_EQ(result, "C5H66P35CPJMGQBADDM6QRJFE1ON4SRKELR7EU3PF8MLUC1H68PJ8D9M6SS3IBP17T0K4GQ48L34\r\nEI29995KOJAE9T852KIJAHALCLQOB5D0====");
}

TEST(base32_tests, encode_option)
{
    std::string result = _base32encode("hello world", CODECBaseNPadding, 0L);
    EXPECT_EQ(result, "NBSWY3DPEB3W64TMMQ");
    
    result = _base32encode("abcdefghijklmnopqrstuvwxyz-_0123456789/!?ABCDEFGHIJKLMNOPQRSTUVWXYZ", CODECBaseNChunkled, 0L);
    EXPECT_EQ(result, "MFRGGZDFMZTWQ2LKNNWG23TPOBYXE43UOV3HO6DZPIWV6MBRGIZTINJWG44DSLZBH5AUEQ2EIVDEOSCJJJFUYTKOJ5IFCUSTKRKVMV2YLFNA====");
}

TEST(base32_tests, decode_standard)
{
    CODECode code;
    std::string result = _base32decode("NBSWY3DPEB3W64TMMQ======", code);
    EXPECT_EQ(result, "hello world");
    
    result = _base32decode("NBSWY3DPEB3W64TMMQ", code);
    EXPECT_EQ(result, "hello world");
    
    result = _base32decode("MFRGGZDFMZTWQ2LKNNWG23TPOBYXE43UOV3HO6DZPIWV6MBRGIZTINJWG44DSLZBH5AUEQ2EIVDE\r\nOSCJJJFUYTKOJ5IFCUSTKRKVMV2YLFNA====", code);
    EXPECT_EQ(result, "abcdefghijklmnopqrstuvwxyz-_0123456789/!?ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    
    result = _base32decode("MFRGGZDFMZTWQ2LKNNWG23TPOBYXE43UOV3HO6DZPIWV6MBRGIZTINJWG44DSLZBH5AUEQ2EIVDEOSCJJJFUYTKOJ5IFCUSTKRKVMV2YLFNA====", code);
    EXPECT_EQ(result, "abcdefghijklmnopqrstuvwxyz-_0123456789/!?ABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

TEST(base32_tests, decode_hex)
{
    CODECode code;
    std::string result = _base32decodehex("D1IMOR3F41RMUSJCCG", code, 1L);
    EXPECT_EQ(result, "hello world");
    
    result = _base32decodehex("C5H66P35CPJMGQBADDM6QRJFE1ON4SRKELR7EU3PF8MLUC1H68PJ8D9M6SS3IBP17T0K4GQ48L34EI29995KOJAE9T852KIJAHALCLQOB5D0====", code, 1L);
    EXPECT_EQ(result, "abcdefghijklmnopqrstuvwxyz-_0123456789/!?ABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

TEST(base32_tests, decode_ignorecase)
{
    CODECode code;
    std::string result = _base32decode_ignorecase("NbswY3dPeB3W64tMmQ======", code);
    EXPECT_EQ(result, "hello world");
}

#endif