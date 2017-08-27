#include <atomic>
#include "gtest/gtest.h"

#include "mgjson.h"

class TestSharedDataData : public _mgjson_shared_data
{
public:
    TestSharedDataData() :
        copy_constuctor_called_(false),
        value_(0)
    {
        counter_++;
    }

    TestSharedDataData(const TestSharedDataData& other):
        copy_constuctor_called_(true),
        value_(other.value_)
    {
        counter_++;
    }

    ~TestSharedDataData()
    {
        counter_--;
    }

public:
    bool copy_constuctor_called_;
    int value_;
    static std::atomic<int> counter_;
};

std::atomic<int> TestSharedDataData::counter_(0);

class TestSharedData
{
public:
    TestSharedData() :
        d(new TestSharedDataData)
    {
    }

    TestSharedData(const TestSharedData &other) :
        d(other.d)
    {
    }

    TestSharedData &operator=(const TestSharedData &other)
    {
        if (this != &other)
            d.operator=(other.d);
        return *this;
    }

    ~TestSharedData()
    {
    }

public:
    bool is_copied() const
    {
        return d->copy_constuctor_called_;
    }

    bool is_same(const TestSharedData &other) const
    {
        return (&(*d)) == (&(*other.d));
    }

    int value() const
    {
        return d->value_;
    }

    void setValue(int new_value)
    {
        d->value_ = new_value;
    }


private:
    _mgjson_shared_data_ptr<TestSharedDataData> d;
};

class SharedDataTest : public ::testing::Test
{
protected:
    void TearDown()
    {
        EXPECT_EQ(TestSharedDataData::counter_, 0);
    }
};

TEST_F(SharedDataTest, CreateAndDestroy)
{
    TestSharedData a;
    EXPECT_EQ(a.value(), 0);
    EXPECT_EQ(a.is_copied(), false);
}

TEST_F(SharedDataTest, Copy)
{
    TestSharedData a;
    EXPECT_EQ(a.value(), 0);
    TestSharedData b(a);
    EXPECT_EQ(b.value(), 0);

    EXPECT_EQ(a.is_copied(), false);
    EXPECT_EQ(b.is_copied(), false);
    EXPECT_EQ(a.is_same(b), true);
}

TEST_F(SharedDataTest, Assignment)
{
    TestSharedData a;
    EXPECT_EQ(a.value(), 0);
    TestSharedData b;
    EXPECT_EQ(b.value(), 0);
    b = a;
    EXPECT_EQ(b.value(), 0);

    EXPECT_EQ(a.is_copied(), false);
    EXPECT_EQ(b.is_copied(), false);
    EXPECT_EQ(a.is_same(b), true);
}

TEST_F(SharedDataTest, AssignmentAndCopyMixed)
{
    TestSharedData a;
    EXPECT_EQ(a.value(), 0);
    TestSharedData b;
    EXPECT_EQ(b.value(), 0);
    b = a;
    EXPECT_EQ(b.value(), 0);
    TestSharedData c(a);
    EXPECT_EQ(c.value(), 0);

    EXPECT_EQ(a.is_copied(), false);
    EXPECT_EQ(b.is_copied(), false);
    EXPECT_EQ(c.is_copied(), false);
    EXPECT_EQ(a.is_same(b), true);
    EXPECT_EQ(a.is_same(c), true);
}

TEST_F(SharedDataTest, NoCopyChange)
{
    TestSharedData a;
    EXPECT_EQ(a.value(), 0);
    a.setValue(1);
    EXPECT_EQ(a.value(), 1);
    a.setValue(2);
    EXPECT_EQ(a.value(), 2);

    EXPECT_EQ(a.is_copied(), false);
}

TEST_F(SharedDataTest, SimpleCopyOnWrite)
{
    TestSharedData a;
    EXPECT_EQ(a.value(), 0);
    TestSharedData b(a);
    EXPECT_EQ(b.value(), 0);
    EXPECT_EQ(a.is_copied(), false);
    EXPECT_EQ(b.is_copied(), false);
    EXPECT_EQ(a.is_same(b), true);

    b.setValue(1);
    EXPECT_EQ(b.value(), 1);
    EXPECT_EQ(a.is_copied(), false);
    EXPECT_EQ(b.is_copied(), true);
}

TEST_F(SharedDataTest, TripleCopyOnWrite)
{
    TestSharedData a;
    EXPECT_EQ(a.value(), 0);
    TestSharedData b(a);
    EXPECT_EQ(b.value(), 0);
    EXPECT_EQ(a.is_copied(), false);
    EXPECT_EQ(b.is_copied(), false);
    EXPECT_EQ(a.is_same(b), true);

    b.setValue(1);
    EXPECT_EQ(b.value(), 1);
    EXPECT_EQ(a.is_copied(), false);
    EXPECT_EQ(b.is_copied(), true);

    TestSharedData c;
    c = a;
    EXPECT_EQ(c.value(), 0);
    EXPECT_EQ(c.is_copied(), false);
    EXPECT_EQ(c.is_same(a), true);
    EXPECT_EQ(c.is_same(b),false);

    c.setValue(2);
    EXPECT_EQ(c.value(), 2);
    EXPECT_EQ(c.is_copied(), true);
}
