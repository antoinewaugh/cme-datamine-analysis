//
// Created by Antoine Waugh on 24/04/2017.
//
#include "../src/DepthBook.h"
#include <gtest/gtest.h>

namespace {

class DepthBookTest : public testing::Test {
public:
    DepthBook depthbook;
    DepthBookTest()
        : depthbook("ESM7", "ES")
    {
    }
};

TEST_F(DepthBookTest, handleMessage_validTimestamp)
{

    const std::string s = "1128=9\u00019=174\u000135=X\u000149=CME\u000175=20170403\u000134=1061\u000152=20170402211926965794928\u000160=20170402211926965423233\u00015799=10000100\u0001268=1\u0001279=1\u0001269=1\u000148=9006\u000155=ESM7\u000183=118\u0001270=236025.0\u0001271=95\u0001346=6\u00011023=9\u000110=088\u0001";
    depthbook.handleMessage(s);
    ASSERT_EQ(depthbook.getTimestamp(), "20170402211926965423233");
}
}
