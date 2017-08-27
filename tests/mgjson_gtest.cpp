#include <atomic>
#include "gtest/gtest.h"

TEST(test1, test_subtest_1)
{
    std::atomic<int> a;
    a = 3;
    EXPECT_GT(a.load(), 2);
}
