#pragma once
#ifndef _MGJSON_H_INCLUDED_
#define _MGJSON_H_INCLUDED_

#ifdef QT_CORE_LIB
#include <QSharedDataPointer>
typedef QSharedData _mgjson_shared_data;
template <class T>
class _mgjson_shared_data_ptr : public QSharedDataPointer<T> {};
#include <QString>
#else
#include "mgjson_shared_data.h"
#endif

class mgjson_private;
class mgjson
{
public:
    struct parse_result
    {
        enum parse_error {
            NoError                 = 0,
            MoreData                = 1,
            InvalidCharacter        = (-1),
            EndOfData               = (-2),
            IntExpected             = (-3),
            InvalidNumber           = (-4),
            SquareBracketExpected   = (-5),
            CurlyBracketExpected    = (-6),
            ColonExpected           = (-7),
            InvalidName             = (-8),
            DuplicateName           = (-9),
        };

        parse_error error;
        int offset, col,row;

        static const char* error_string(parse_error err);

        inline const char* error_string() const
        {
            return error_string(error);
        }

        inline bool isOk() const
        {
            return (error >= 0);
        }
    };

private:
    _mgjson_shared_data_ptr<mgjson_private> d;
};

#ifdef QT_CORE_LIB
struct GJsonParseError : public mgjson::parse_result
{
    typedef parse_error ParseError;
    static inline QString errorString(ParseError err)
    {
        return QString::fromUtf8(error_string(err));
    }

    inline QString errorString() const
    {
        return QString::fromUtf8(error_string(error));
    }
};
#endif

#endif // _MGJSON_H_INCLUDED_


