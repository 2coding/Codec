//
//  main.cpp
//  Tests
//
//  Created by hesy on 15/11/17.
//  Copyright © 2015年 2Coding. All rights reserved.
//

#include <iostream>
#include <gtest/gtest.h>
#include "codec.h"

static std::string _base64Encode(const std::string &data) {
    cdc_base64 *base64 = base64_init(base64opt_standard);
    size_t buflen = base64_encode_length(base64, data.size());
    char *buf = new char[buflen + 1];
    base64_encode(base64, (unsigned char *)data.data(), data.size(), buf, &buflen);
    
    std::string ret(buf, buflen);
    delete [] buf;
    base64_cleanup(base64);
    
    return ret;
}

TEST(base64_tests, test)
{
    std::string result = _base64Encode("hello world");
    EXPECT_EQ(result, "aGVsbG8gd29ybGQ=");
}

int main(int argc, const char * argv[]) {
    testing::InitGoogleTest(&argc, (char **)argv);
    return RUN_ALL_TESTS();
}
