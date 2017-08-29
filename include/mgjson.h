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
#   include <string>
#   include <list>
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
        int offset, col, row;

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

public:
    json_type type() const;
    inline bool is_null() const { return type() == Null; }
    inline bool is_bool() const { return type() == Bool; }
    inline bool is_integer() const { return type() == Integer; }
    inline bool is_double() const { return type() == Double; }
    inline bool is_string() const { return type() == String; }
    inline bool is_array() const { return type() == Array; }
    inline bool is_object() const { return type() == Object; }
    inline bool is_undefined() const { return type() == Undefined; }
    inline bool is_numeric() const { json_type t = type(); return (Double == t) || (Integer == t); }
    inline bool is_compound() const { json_type t = type(); return (Object == t) || (Array == t); }
    inline bool is_set() const { json_type t = type(); return (Null != t) && (Undefined != t); }

public:
    bool to_bool() const;
    int to_int() const;
    unsigned int to_uint() const;
    long long to_longlong() const;
    unsigned long long to_ulonglong() const;
    float to_float() const;
    double to_double() const;
    const char* to_str() const;
    std::string to_string() const;

    inline operator bool () const { return to_bool(); }
    inline operator int () const { return to_int(); }
    inline operator unsigned int () const { return to_uint(); }
    inline operator long long () const { return to_longlong(); }
    inline operator unsigned long long () const { return to_ulonglong(); }
    inline operator float () const { return to_float(); }
    inline operator double () const { return to_double(); }
    inline operator const char* () const { return to_str(); }
    inline operator std::string () const { return to_string(); }

    template<typename T>
    inline T to() const { return (T) (*this); }

public:
    size_t count() const;
    void resize(size_t new_size);

    mgjson at(size_t index) const;
    mgjson& at(size_t index);
    inline mgjson operator [](size_t index) const { return at(index); }
    inline mgjson& operator [](size_t index) { return at(index); }

    mgjson at(const char* key) const;
    mgjson& at(const char* key);
    inline mgjson at(const std::string& key) const { return at(key.c_str()); }
    inline mgjson& at(const std::string& key) { return at(key.c_str()); }
    inline mgjson operator [](const char* key) const { return at(key); }
    inline mgjson& operator [](const char* key) { return at(key); }

    std::list<std::string> keys() const;

public:
    void append(const mgjson& value);
    void prepend(const mgjson& value);

    void remove(size_t index);
    void remove(const char* key);
    inline void remove(const std::string& key) { remove(key.c_str()); }

    mgjson take(size_t index);
    mgjson take(const char* key);
    inline mgjson take(const std::string& key) { return take(key.c_str()); }

public:
    std::string to_json(json_format format = MaxReadable) const;

    static mgjson from_json(const char *data, size_t cb_data, parse_result *result = nullptr);
    static mgjson from_json(const char *data, parse_result *result = nullptr);
    static inline mgjson from_json(const std::string& data, parse_result *result = nullptr)
    {
        return from_json(data.data(), data.size(), result);
    }

#ifdef MGJSON_USE_MSGPACK
public:
    std::string msgpack() const;

    static mgjson msgunpack(const char *data, size_t cb_data, parse_result *result = nullptr);
    static inline mgjson msgunpack(const std::string& data, parse_result *result = nullptr)
    {
        return msgunpack(data.data(), data.size(), result);
    }
#endif  // MGJSON_USE_MSGPACK

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

public:
    inline bool isNull() const { return is_null(); }
    inline bool isBool() const { return is_bool(); }
    inline bool isInteger() const { return is_integer(); }
    inline bool isDouble() const { return is_double(); }
    inline bool isNumeric() const { return is_numeric(); }
    inline bool isString() const { return is_string(); }
    inline bool isArray() const { return is_array(); }
    inline bool isObject() const { return is_object(); }
    inline bool isCompound() const { return is_compound(); }
    inline bool isUndefined() const { return is_undefined(); }
    inline bool isSet() const { return is_set(); }

public:
    inline bool toBool() const { return to_bool(); }
    inline int toInt() const { return to_int(); }
    inline unsigned int toUInt() const { return to_uint(); }
    inline long long toLongLong() const { return to_longlong(); }
    inline unsigned long long toULongLong() const { return to_ulonglong(); }
    inline float toFloat() const { return to_float(); }
    inline double toDouble() const { return to_double(); }
    inline const char* toStr() const { return to_str(); }
    inline std::string toStdString() const { return to_string(); }

    QString toString() const;
    QByteArray toByteArray() const;
    QVariant toVariant() const;

    inline operator QString () const { return toString(); }
    inline operator QByteArray () const { return toByteArray(); }
    inline operator QVariant () const { return toVariant(); }

public:
    inline int count() const { return static_cast<int>(mgjson::count()); }
    inline void resize(int new_size) { mgjson::resize(static_cast<size_t>(new_size)); }

    inline mgjson at(int index) const { return mgjson::at(static_cast<int>(index)); }
    inline mgjson& at(int index) { return mgjson::at(static_cast<int>(index)); }
    inline mgjson operator [](int index) const { return at(index); }
    inline mgjson& operator [](int index) { return at(index); }

    inline mgjson at(const QByteArray& key) const { return mgjson::at(key.constData()); }
    inline mgjson& at(const QByteArray& key) { return mgjson::at(key.constData()); }
    inline mgjson operator [](const QByteArray& key) const { return mgjson::at(key.constData()); }
    inline mgjson& operator [](const QByteArray& key) { return mgjson::at(key.constData()); }

    inline mgjson at(const QString& key) const { return mgjson::at(key.toUtf8().constData()); }
    inline mgjson& at(const QString& key) { return mgjson::at(key.toUtf8().constData()); }
    inline mgjson operator [](const QString& key) const { return mgjson::at(key.toUtf8().constData()); }
    inline mgjson& operator [](const QString& key) { return mgjson::at(key.toUtf8().constData()); }

    QList<QByteArray> keys() const;

public:
    inline void removeAt(int index) { mgjson::remove(static_cast<size_t>(index)); }
    inline void removeAt(const char* key) { mgjson::remove(key); }
    inline void removeAt(const std::string& key) { mgjson::remove(key.c_str()); }
    inline void removeAt(const QByteArray& key) { mgjson::remove(key.constData()); }
    inline void removeAt(const QString& key) { mgjson::remove(key.toUtf8().constData()); }

    inline mgjson takeAt(int index) { return mgjson::take(static_cast<size_t>(index)); }
    inline mgjson takeAt(const char* key) { return mgjson::take(key); }
    inline mgjson takeAt(const std::string& key) { return mgjson::take(key.c_str()); }
    inline mgjson takeAt(const QByteArray& key) { return mgjson::take(key.constData()); }
    inline mgjson takeAt(const QString& key) { return mgjson::take(key.toUtf8().constData()); }

public:
    QByteArray toJson(JsonFormat format = MaxReadable) const;
    inline static mgjson fromJson(const char *data, size_t cb_data, parse_result *result = nullptr)
    {
        return mgjson::from_json(data, cb_data, result);
    }

    inline static mgjson fromJson(const char *data, parse_result *result = nullptr)
    {
        return mgjson::from_json(data, result);
    }

    static inline mgjson fromJson(const std::string& data, parse_result *result = nullptr)
    {
        return mgjson::from_json(data.data(), data.size(), result);
    }

    static inline mgjson fromJson(const QByteArray& data, parse_result *result = nullptr)
    {
        return mgjson::from_json(data.constData(), data.size(), result);
    }

#ifdef MGJSON_USE_MSGPACK
public:
    QByteArray msgpack() const;

    static inline mgjson msgunpack(const QByteArray& data, parse_result *result = nullptr)
    {
        return mgjson::msgunpack(data.constData(), data.size(), result);
    }
#endif  // MGJSON_USE_MSGPACK
};

template<>
inline QString mgjson::to() const
{
  return static_cast<const GJson*>(this)->toString();
}

template<>
inline QByteArray mgjson::to() const
{
  return static_cast<const GJson*>(this)->toByteArray();
}

Q_DECLARE_METATYPE(mgjson)
Q_DECLARE_METATYPE(GJson)

#endif  // QT_CORE_LIB

#endif // _MGJSON_H_INCLUDED_
