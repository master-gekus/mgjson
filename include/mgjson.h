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
#   include <vector>
#   include "mgjson_shared_data.h"
#   define _mgjson_declare_flags(Flags, Enum) typedef unsigned int Flags;
#   define _mgjson_declare_operators_for_flags(Flags)
#endif  // QT_CORE_LIB

#include <type_traits>

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

#ifdef QT_CORE_LIB
public:
    typedef json_type Type;
    typedef json_format JsonFormat;
#endif

private:
    mgjson(mgjson_private *data) noexcept;

public:
    mgjson(json_type type = Null) noexcept;
    mgjson(const mgjson &other) noexcept;
    mgjson(mgjson &&other) noexcept;
    mgjson(bool value) noexcept;
    mgjson(int value) noexcept;
    mgjson(unsigned int value) noexcept;
    mgjson(long value) noexcept;
    mgjson(unsigned long value) noexcept;
    mgjson(long long value) noexcept;
    mgjson(unsigned long long value) noexcept;
    mgjson(float value) noexcept;
    mgjson(double value) noexcept;
    mgjson(long double value) noexcept;
    mgjson(const char* value) noexcept;
    mgjson(const std::string& value) noexcept;
    mgjson &operator=(const mgjson& other) noexcept;

#ifdef QT_CORE_LIB
    mgjson(const QString &value) noexcept;
    mgjson(const QByteArray &value) noexcept;
    mgjson(const QVariant &value) noexcept;
#endif

public:
    ~mgjson() noexcept;

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

#ifdef QT_CORE_LIB
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
#endif

public:
    bool to_bool() const;
    inline char to_char() const { return static_cast<char>(to_ulonglong()); }
    inline unsigned char to_uchar() const { return static_cast<unsigned char>(to_ulonglong()); }
    inline short to_short() const { return static_cast<short>(to_ulonglong()); }
    inline unsigned short to_ushort() const { return static_cast<unsigned short>(to_ulonglong()); }
    inline int to_int() const { return static_cast<int>(to_ulonglong()); }
    inline unsigned int to_uint() const { return static_cast<unsigned int>(to_ulonglong()); }
    inline long to_long() const { return static_cast<long>(to_ulonglong()); }
    inline unsigned long to_ulong() const { return static_cast<unsigned long>(to_ulonglong()); }
    inline long long to_longlong() const { return static_cast<long long>(to_ulonglong()); }
    unsigned long long to_ulonglong() const;
    long double to_longdouble() const;
    inline float to_float() const { return static_cast<float>(to_longdouble()); }
    inline double to_double() const { return static_cast<double>(to_longdouble()); }
    const char* to_str() const;

#ifndef QT_CORE_LIB
    const std::string& to_string() const;
#else
    std::string to_string() const;

    inline bool toBool() const { return to_bool(); }
    inline char toChar() const { return to_char(); }
    inline unsigned char toUChar() const { return to_uchar(); }
    inline short toShort() const { return to_short(); }
    inline unsigned short toUShort() const { return to_ushort(); }
    inline int toInt() const { return to_int(); }
    inline unsigned int toUInt() const { return to_uint(); }
    inline long toLong() const { return to_long(); }
    inline unsigned long toULong() const { return to_ulong(); }
    inline long long toLongLong() const { return to_longlong(); }
    inline unsigned long long toULongLong() const { return to_ulonglong(); }
    inline float toFloat() const { return to_float(); }
    inline double toDouble() const { return to_double(); }
    inline long double toLongDouble() const { return to_longdouble(); }
    inline const char* toStr() const { return to_str(); }
    inline std::string toStdString() const { return to_string(); }
    const QByteArray& toByteArray() const;
    QString toString() const;
    QVariant toVariant() const;
#endif

public:
    template <typename T>
    typename std::enable_if<std::is_integral<T>::value, T>::type
    inline to() const {return static_cast<T>(to_ulonglong());}

    template <typename T>
    typename std::enable_if<std::is_floating_point<T>::value, T>::type
    inline to() const {return static_cast<T>(to_longdouble());}

    template <typename T>
    typename std::enable_if<(!std::is_integral<T>::value) && (!std::is_floating_point<T>::value), T>::type
    to() const;

    template <typename T>
    inline operator T () const {return to<T>();}

public:
#ifdef QT_CORE_LIB
    int
#else
    size_t
#endif
    count() const;

    void resize(size_t new_size);
#ifdef QT_CORE_LIB
    inline void resize(int new_size) { resize(static_cast<size_t>(new_size)); }
#endif

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
    inline mgjson operator [](const std::string& key) const { return at(key.c_str()); }
    inline mgjson& operator [](const std::string& key) { return at(key.c_str()); }

    bool has_key(const char* key) const;
    inline bool has_key(const std::string& key) const { return has_key(key.c_str()); }

#ifdef QT_CORE_LIB
    inline mgjson at(int index) const { return at(static_cast<size_t>(index)); }
    inline mgjson& at(int index) { return at(static_cast<size_t>(index)); }
    inline mgjson operator [](int index) const { return at(index); }
    inline mgjson& operator [](int index) { return at(index); }

    inline mgjson at(const QByteArray& key) const { return at(key.constData()); }
    inline mgjson& at(const QByteArray& key) { return at(key.constData()); }
    inline mgjson operator [](const QByteArray& key) const { return at(key.constData()); }
    inline mgjson& operator [](const QByteArray& key) { return at(key.constData()); }

    inline mgjson at(const QString& key) const { return at(key.toUtf8().constData()); }
    inline mgjson& at(const QString& key) { return at(key.toUtf8().constData()); }
    inline mgjson operator [](const QString& key) const { return at(key.toUtf8().constData()); }
    inline mgjson& operator [](const QString& key) { return at(key.toUtf8().constData()); }

    inline bool hasKey(const char* key) const { return has_key(key); }
    inline bool hasKey(const std::string& key) const { return has_key(key.c_str()); }
    inline bool hasKey(const QByteArray& key) const { return has_key(key.constData()); }
    inline bool hasKey(const QString& key) const { return has_key(key.toUtf8().constData()); }

    QByteArrayList keys() const;
#else
    std::vector<std::string> keys() const;
#endif

public:
    mgjson& push_back(const mgjson& value);
    mgjson& push_front(const mgjson& value);
    inline mgjson& push_back() {return push_back(mgjson());}
    inline mgjson& push_front() {return push_front(mgjson());}

    void remove(size_t index);
    void remove(const char* key);
    inline void remove(const std::string& key) { remove(key.c_str()); }

    mgjson take(size_t index);
    mgjson take(const char* key);
    inline mgjson take(const std::string& key) { return take(key.c_str()); }

#ifdef QT_CORE_LIB
    inline mgjson& append(const mgjson& value) {return push_back(value);}
    inline mgjson& prepend(const mgjson& value) {return push_front(value);}
    inline mgjson& append() {return push_back(mgjson());}
    inline mgjson& prepend() {return push_front(mgjson());}

    inline void removeAt(int index) { remove(static_cast<size_t>(index)); }
    inline void removeAt(const char* key) { remove(key); }
    inline void removeAt(const std::string& key) { remove(key.c_str()); }
    inline void removeAt(const QByteArray& key) { remove(key.constData()); }
    inline void removeAt(const QString& key) { remove(key.toUtf8().constData()); }

    inline mgjson takeAt(int index) { return take(static_cast<size_t>(index)); }
    inline mgjson takeAt(const char* key) { return take(key); }
    inline mgjson takeAt(const std::string& key) { return take(key.c_str()); }
    inline mgjson takeAt(const QByteArray& key) { return take(key.constData()); }
    inline mgjson takeAt(const QString& key) { return take(key.toUtf8().constData()); }
#endif

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

static_assert(sizeof(mgjson) == sizeof(void*),
              "mgjason class MUST have size as a pointer!");

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

typedef mgjson GJson;

//class GJson : public mgjson
//{
//public:
//    QByteArray toJson(JsonFormat format = MaxReadable) const;
//    inline static mgjson fromJson(const char *data, size_t cb_data, parse_result *result = nullptr)
//    {
//        return mgjson::from_json(data, cb_data, result);
//    }

//    inline static mgjson fromJson(const char *data, parse_result *result = nullptr)
//    {
//        return mgjson::from_json(data, result);
//    }

//    static inline mgjson fromJson(const std::string& data, parse_result *result = nullptr)
//    {
//        return mgjson::from_json(data.data(), data.size(), result);
//    }

//    static inline mgjson fromJson(const QByteArray& data, parse_result *result = nullptr)
//    {
//        return mgjson::from_json(data.constData(), data.size(), result);
//    }

//#ifdef MGJSON_USE_MSGPACK
//public:
//    QByteArray msgpack() const;

//    static inline mgjson msgunpack(const QByteArray& data, parse_result *result = nullptr)
//    {
//        return mgjson::msgunpack(data.constData(), data.size(), result);
//    }
//#endif  // MGJSON_USE_MSGPACK
//};

template<>
inline QString mgjson::to<QString>() const
{
  return toString();
}

template<>
inline QByteArray mgjson::to<QByteArray>() const
{
  return toByteArray();
}

template<>
inline const QByteArray& mgjson::to<const QByteArray&>() const
{
  return toByteArray();
}

Q_DECLARE_TYPEINFO(mgjson, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(mgjson)

#else  // QT_CORE_LIB
template <>
inline const std::string& mgjson::to<const std::string&>() const
{
    return to_string();
}
#endif  // QT_CORE_LIB

template <>
inline bool mgjson::to<bool>() const
{
    return to_bool();
}

template <>
inline const char* mgjson::to<const char*>() const
{
    return to_str();
}

template <>
inline std::string mgjson::to<std::string>() const
{
    return to_string();
}

#endif // _MGJSON_H_INCLUDED_
