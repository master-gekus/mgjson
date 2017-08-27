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
    inline _mgjson_shared_data_ptr(const _mgjson_shared_data_ptr<T> &o) :
        d(o.d)
    {
        if(d) {
            ++d->ref;
        }
    }

    inline explicit _mgjson_shared_data_ptr(T *data) :
        d(data)
    {
        if(d) {
            d->ref++;
        }
    }

    inline ~_mgjson_shared_data_ptr()
    {
        if (d && (0 == (--d->ref))) {
            delete d;
        }
    }

public:
    inline const T &operator*() const { return *d; }
    inline const T *operator->() const { return d; }
    inline T &operator*() { detach(); return *d; }
    inline T *operator->() { detach(); return d; }

    inline _mgjson_shared_data_ptr<T> & operator=(const _mgjson_shared_data_ptr<T> &o)
    {
        if(o.d != d) {
            if(o.d) {
                ++o.d->ref;
            }
            T *old = d;
            d = o.d;
            if (old && (0 == (--old->ref))) {
                delete old;
            }
        }
        return *this;
    }

private:
    inline void detach()
    {
        if (d && (d->ref != 1)) {
            T *x = new T(*d);
            ++x->ref;
            if(0 == (--d->ref)) {
                delete d;
            }
            d = x;
        }
    }

private:
    T *d;
};

#endif // _MGJSON_SHARED_DATA_H_INCLUDED
