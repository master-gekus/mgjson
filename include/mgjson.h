#pragma once
#ifndef _MGJSON_H_INCLUDED_
#define _MGJSON_H_INCLUDED_

#ifndef QT_CORE_LIB
#include "mgjson_shared_data.h"
#else
#include <QSharedDataPointer>
typedef QSharedData _mgjson_shared_data;
template <class T>
class _mgjson_shared_data_ptr : public QSharedDataPointer<T> {};
#endif

class mgjson_private;
class mgjson
{

private:
    _mgjson_shared_data_ptr<mgjson_private> d;
};


#endif // _MGJSON_H_INCLUDED_


