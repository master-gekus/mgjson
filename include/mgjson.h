#pragma once
#ifndef _MGJSON_H_INCLUDED_
#define _MGJSON_H_INCLUDED_

#ifdef QT_CORE_LIB
#   include <QString>
#   include <QVariant>
#   include <QSharedDataPointer>
    template <class T>
    using _mgjson_shared_data_ptr = QSharedDataPointer<T>;
    typedef QSharedData _mgjson_shared_data;
#   define _mgjson_declare_flags(Flags, Enum) Q_DECLARE_FLAGS(Flags, Enum)
#   define _mgjson_declare_operators_for_flags(Flags) Q_DECLARE_OPERATORS_FOR_FLAGS(Flags)
#else   // QT_CORE_LIB
#   include "mgjson_shared_data.h"
#   define _mgjson_declare_flags(Flags, Enum) typedef unsigned int Flags;
#   define _mgjson_declare_operators_for_flags(Flags)
#endif  // QT_CORE_LIB

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

    enum json_type {
        Null        = 0x0,
        Bool        = 0x1,
        Integer     = 0x2,
        Double      = 0x3,
        String      = 0x4,
        Array       = 0x5,
        Object      = 0x6,
        Undefined   = (-1)
    };

    enum json_format_flags {
        Compact             = 0x0000,
        Indented            = 0x0001,
        UseSpaces           = 0x0002,
        AlignObjects        = 0x0004,
        SplitStrings        = 0x0008,
        InlineEmptyArrays   = 0x0010,
        InlineEmptyObjects  = 0x0020,
        InlineSimpleArrays  = 0x0040,
        SplitSimpleArrays   = 0x0080,
        SimpleFieldsFirst   = 0x0100,

        MinSize             = Compact,
        MaxReadable         = (
            Indented |
            AlignObjects |
            SplitStrings |
            InlineEmptyArrays |
            InlineEmptyObjects |
            InlineSimpleArrays |
            SplitSimpleArrays |
            SimpleFieldsFirst
        ),
    };
    _mgjson_declare_flags(json_format, json_format_flags)

private:
    mgjson(mgjson_private *data);

public:
    mgjson(json_type type = Null);
    mgjson(const mgjson &other);
    mgjson(bool value);
    mgjson(int value);
    mgjson(unsigned int value);
    mgjson(long long value);
    mgjson(unsigned long long value);
    mgjson(float value);
    mgjson(double value);
    mgjson(const char* value);
    mgjson &operator=(const mgjson& other);

public:
    ~mgjson();

private:
    _mgjson_shared_data_ptr<mgjson_private> d;
};

_mgjson_declare_operators_for_flags(mgjson::json_format)

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

class GJson : public mgjson
{
public:
    typedef json_type Type;
    typedef json_format JsonFormat;

public:
    using mgjson::mgjson;
    inline GJson(Type type = Null) : mgjson(type) {}
    GJson(const QString &value);
    GJson(const QByteArray &value);
    GJson(const QVariant &value);
};

#endif  // QT_CORE_LIB

#endif // _MGJSON_H_INCLUDED_
