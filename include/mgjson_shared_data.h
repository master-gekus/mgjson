#pragma once
#ifndef _MGJSON_SHARED_DATA_H_INCLUDED
#define _MGJSON_SHARED_DATA_H_INCLUDED

#include <atomic>

class _mgjson_shared_data
{
public:
    inline _mgjson_shared_data() : ref(0) {}
    inline _mgjson_shared_data(const _mgjson_shared_data&) : ref(0) {}

private:
    _mgjson_shared_data &operator=(const _mgjson_shared_data&) = delete;

public:
    mutable std::atomic<int> ref;
};

template <class T>
class _mgjson_shared_data_ptr
{
public:
    inline _mgjson_shared_data_ptr() : d(nullptr) {}
    inline explicit _mgjson_shared_data_ptr(T *data) :
        d(data)
    {
        if(d) {
            d->ref++;
        }
    }

    inline ~_mgjson_shared_data_ptr()
    {
        if (d && (0 == (d->ref--))) delete d;
    }

public:
    inline const T &operator*() const { return *d; }
    inline const T *operator->() const { return d; }

private:
    T *d;
};

#endif // _MGJSON_SHARED_DATA_H_INCLUDED
