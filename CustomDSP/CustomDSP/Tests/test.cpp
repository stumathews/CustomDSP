#include "pch.h"

using namespace std;


class VectorTest : public ::testing::Test {
protected:
    void SetUp() override 
    {
        v1.push_back(33);
    }

    void TearDown() override {}

    vector<int> v1;
};

TEST_F(VectorTest, VectorContains33) {
    EXPECT_EQ(v1.front(), 33);
}

//TEST(TestCaseName, TestName) {
//  EXPECT_EQ(1, 1);
//  EXPECT_TRUE(true);
//}
//
//TEST(CircularBufferTests, CircularBufferTest1) {
//    EXPECT_EQ(1, 1);
//    EXPECT_TRUE(true);
//}
//
//TEST(CircularBufferTests, CircularBufferTest2) {
//    EXPECT_EQ(1, 1);
//    EXPECT_TRUE(true);
//}