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
    }

    TestSharedDataData(const TestSharedDataData& other):
        copy_constuctor_called_(true),
        value_(other.value_)
    {
    }

public:
    bool copy_constuctor_called_;
    int value_;
};

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

    void setValue(int new_value);

private:
    _mgjson_shared_data_ptr<TestSharedDataData> d;
};

TEST(SharedData, CreateAndDestroy)
{
    TestSharedData a;
    EXPECT_EQ(a.value(), 0);
    EXPECT_EQ(a.is_copied(), false);
    EXPECT_EQ(a.is_same(a), true);
}
