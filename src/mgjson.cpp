#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "mgjson.h"

#include <string>
#include <cstring>
#include <cstdlib>
#include <map>
#include <vector>
#include <limits>
#include <stdexcept>
#include <cassert>

#ifndef QT_CORE_LIB
char *qstrdup(const char *src)
{
    if (nullptr == src) {
        return 0;
    }
    size_t len = strlen(src);
    char *dest = new char[len + 1];
    memcpy(dest, src, len + 1);
    return dest;
}

int qstricmp(const char *str1, const char *str2)
{
#ifdef _MSC_VER
    return _stricmp(str1, str2);
#else
    return strcasecmp(str1, str2);
#endif
}

#endif

const char*
mgjson::parse_result::error_string(parse_error err)
{
    switch(err)
    {
    case NoError:               return "No error.";
    case MoreData:              return "Where is more data.";
    case InvalidCharacter:      return "Invalid character.";
    case EndOfData:             return "Unexpected end of data.";
    case IntExpected:           return "Expected integer value.";
    case InvalidNumber:         return "Invalid number representation.";
    case SquareBracketExpected: return "Square bracket expected.";
    case CurlyBracketExpected:  return "Curly bracket expected.";
    case ColonExpected:         return "Colon expected.";
    case InvalidName:           return "Invalid name of object field.";
    case DuplicateName:         return "Duplicate name of object field.";
    default:                    return "<unknown error>";
    }
}

class mgjson_private : public _mgjson_shared_data
{
public:
    ~mgjson_private()
    {
    }

    mgjson_private(const mgjson_private& other) :
        _mgjson_shared_data(other),
        type_(other.type_),
        b_value_(other.b_value_),
        i_value_(other.i_value_),
        d_value_(other.d_value_),
        str_value_(other.str_value_),
        array_(other.array_),
        map_(other.map_)
    {
    }

    mgjson_private(mgjson::json_type type) :
        type_(type),
        b_value_(false),
        i_value_(0),
        d_value_(0.0),
        str_value_(mgjson::Null == type ? "null" : "")
    {
    }

    mgjson_private(bool value) :
        type_(mgjson::Bool),
        b_value_(value),
        i_value_(value ? 1 : 0),
        d_value_(value ? 1.0 : 0.0),
        str_value_(value ? "true" : "false")
    {
    }

    mgjson_private(unsigned long long value) :
        type_(mgjson::Integer),
        b_value_(!!value),
        i_value_(value),
        d_value_(static_cast<long double>(value)),
#ifdef QT_CORE_LIB
        str_value_(QByteArray::number(value))
#else
        str_value_(std::to_string(value))
#endif
    {
    }

    mgjson_private(long double value) :
        type_(mgjson::Double),
        b_value_(0.0L != value),
        i_value_(static_cast<unsigned long long>(value)),
        d_value_(value)
    {
        str_value_.resize(std::numeric_limits<long double>::digits10 + 10);
#ifdef QT_CORE_LIB
        char* buf = str_value_.data();
#else
        char* buf = &str_value_.front();
#endif

#ifdef _MSC_VER
        int len = sprintf_s(buf, str_value_.size(), "%.*Lg",
                  std::numeric_limits<long double>::digits10 + 2, value);
#else
        int len = sprintf(buf, "%.*Lg",
                std::numeric_limits<long double>::digits10 + 2, value);
#endif
        str_value_.resize(len);
    }

    mgjson_private(const char* value) :
        type_(mgjson::String),
        b_value_(false),
        i_value_(0),
        d_value_(0.0),
        str_value_(value)
    {
        _update_values_from_string();
    }

#ifdef QT_CORE_LIB
    mgjson_private(const QByteArray& value) :
#else
    mgjson_private(const std::string& value) :
#endif
        type_(mgjson::String),
        b_value_(false),
        i_value_(0),
        d_value_(0.0),
        str_value_(value)
    {
        _update_values_from_string();
    }

    inline void check_key_is_empty(const char *key) const
    {
        if ((nullptr == key) || (0 == strlen(key))) {
            throw std::out_of_range("mgjson::at(key) key can't be empty!");
        }
    }

    inline bool switch_to_array()
    {
        switch(type_) {
        case mgjson::Undefined:
        case mgjson::Null:
            type_ = mgjson::Array;
        case mgjson::Array:
            return true;
        default:
            return false;
        }
    }

private:
    void _update_values_from_string()
    {
#ifdef MGJSON_AUTOCAST_STRING_VALUES
#   ifdef QT_CORE_LIB
        const char* str = str_value_.constData();
#   else
        const char* str = str_value_.c_str();
#   endif
        if (strlen(str) != static_cast<size_t>(str_value_.size())) {
            return;     // Never cast strings with zeros
        }
        if ((0 == qstricmp("0", str)) || (0 == qstricmp("off", str))
            || (0 == qstricmp("false", str))) {
            return;
        }
        if ((0 == qstricmp("on", str)) || (0 == qstricmp("true", str))) {
            b_value_ = true;
            i_value_ = 1;
            d_value_ = 1.0;
            return;
        }
        char* endptr;
        bool i_value_set = false;
        unsigned long long i_val = strtoull(str, &endptr, 0);
        if ((str + str_value_.size()) == endptr) {
            i_value_ = i_val;
            i_value_set = true;
        }
        long double d_val = strtold(str, &endptr);
        if ((str + str_value_.size()) == endptr) {
            d_value_ = d_val;
            if (!i_value_set) {
                if (0.0 > d_val) {
                    if (static_cast<long double>(std::numeric_limits<long long>::min() > d_val)) {
                        i_value_ = static_cast<unsigned long long>(std::numeric_limits<long long>::min());
                    }
                    else {
                        i_value_ = static_cast<unsigned long long>(static_cast<long long>(d_val));
                    }
                }
                else {
                    if (static_cast<long double>(std::numeric_limits<unsigned long long>::max() < d_val)) {
                        i_value_ = static_cast<unsigned long long>(std::numeric_limits<unsigned long long>::max());
                    }
                    else {
                        i_value_ = static_cast<unsigned long long>(d_val);
                    }
                }
            }
        } else if (i_value_set) {
            d_value_ = static_cast<long double>(i_value_);
        }
        b_value_ = (0 != i_value_);
#endif
    }

public:
    struct Key {
        explicit Key(const char* key) :
            d(qstrdup(key))
        {
            assert(nullptr != key);
        }

        explicit Key(const Key& other) :
            d(qstrdup(other.d))
        {
            assert(nullptr != other.d);
        }

        explicit Key(Key&& other) :
            d(other.d)
        {
            assert(nullptr != other.d);
            other.d = nullptr;
        }

        ~Key()
        {
            delete[] d;
        }

        bool operator <(const Key& other) const
        {
            return (strcmp(d, other.d) < 0);
        }
        char *d;
    };

public:
    mgjson::json_type type_;
    bool b_value_;
    unsigned long long i_value_;
    long double d_value_;
#ifdef QT_CORE_LIB
    QByteArray str_value_;
#else
    std::string str_value_;
#endif
    std::vector<mgjson> array_;
    std::map<Key,mgjson> map_;
};

mgjson::~mgjson() noexcept
{
}

mgjson::mgjson(mgjson_private *data) noexcept :
    d(data)
{
}

mgjson::mgjson(const mgjson& other) noexcept :
    d(other.d)
{
}

mgjson::mgjson(mgjson &&other) noexcept :
    d(other.d)
{
}

mgjson &mgjson::operator=(const mgjson& other) noexcept
{
  if( this != &other ) {
    d.operator=(other.d);
  }
  return *this;
}

mgjson::mgjson(json_type type) noexcept :
    d(new mgjson_private(type))
{
}

mgjson::mgjson(bool value) noexcept :
    d(new mgjson_private(value))
{
}

mgjson::mgjson(int value) noexcept :
    d(new mgjson_private(static_cast<unsigned long long>(value)))
{
}

mgjson::mgjson(unsigned int value) noexcept :
    d(new mgjson_private(static_cast<unsigned long long>(value)))
{
}

mgjson::mgjson(long value) noexcept :
    d(new mgjson_private(static_cast<unsigned long long>(value)))
{
}

mgjson::mgjson(unsigned long value) noexcept :
    d(new mgjson_private(static_cast<unsigned long long>(value)))
{
}

mgjson::mgjson(long long value) noexcept :
    d(new mgjson_private(static_cast<unsigned long long>(value)))
{
}

mgjson::mgjson(unsigned long long value) noexcept :
    d(new mgjson_private(value))
{
}

mgjson::mgjson(float value) noexcept :
    d(new mgjson_private(static_cast<long double>(value)))
{
}

mgjson::mgjson(double value) noexcept :
    d(new mgjson_private(static_cast<long double>(value)))
{
}

mgjson::mgjson(long double value) noexcept :
    d(new mgjson_private(value))
{
}

mgjson::mgjson(const char* value) noexcept :
    d(new mgjson_private(value))
{
}

mgjson::mgjson(const std::string& value) noexcept :
#ifdef QT_CORE_LIB
    d(new mgjson_private(QByteArray::fromStdString(value)))
#else
    d(new mgjson_private(value))
#endif
{
}

#ifdef QT_CORE_LIB
mgjson::mgjson(const QByteArray& value) noexcept :
    d(new mgjson_private(value))
{
}

mgjson::mgjson(const QString& value) noexcept :
    d(new mgjson_private(value.toUtf8()))
{
}
#endif

mgjson::json_type
mgjson::type() const
{
  return d->type_;
}

bool
mgjson::to_bool() const
{
    return d->b_value_;
}

unsigned long long
mgjson::to_ulonglong() const
{
    return d->i_value_;
}

long double
mgjson::to_longdouble() const
{
    return d->d_value_;
}

const char*
mgjson::to_str() const
{
#ifdef QT_CORE_LIB
    return d->str_value_.constData();
#else
    return d->str_value_.c_str();
#endif
}

#ifndef QT_CORE_LIB

const std::string&
mgjson::to_string() const
{
    return d->str_value_;
}

#else

std::string
mgjson::to_string() const
{
    return d->str_value_.toStdString();
}

const QByteArray&
mgjson::toByteArray() const
{
    return d->str_value_;
}

QString
mgjson::toString() const
{
    /* We use fromUtf8(const char*, int) because fromUtf8(const QByteArray&) trancates
     * value by the first '\0'
     */
    const QByteArray& str = d->str_value_;
    return QString::fromUtf8(str.constData(), str.size());
}

#pragma message("TODO: Constructor form QVariant!")
#endif

#ifdef QT_CORE_LIB
int
#else
size_t
#endif
mgjson::count() const
{
    switch(d->type_) {
    case Array:
        return static_cast<decltype(count())>(d->array_.size());
    case Object:
        return static_cast<decltype(count())>(d->map_.size());
    default:
        return 0;
    }
}

void
mgjson::resize(size_t new_size)
{
    if ((Array == d->type_) && (d->array_.size() == new_size)) {
        return;
    }
    mgjson_private *data = d.data();
    data->type_ = Array;
    data->map_.clear();
    data->array_.resize(new_size);
}

mgjson
mgjson::at(size_t index) const
{
    const mgjson_private* data = d.data();
    if ((Array != data->type_) || (data->array_.size() <= index)) {
        return mgjson();
    }
    return data->array_.at(index);
}

mgjson&
mgjson::at(size_t index)
{
    mgjson_private* data = d.data();
    if (!data->switch_to_array()) {
        throw std::invalid_argument("mgjson::at(index) can't be used for json what is not an array.");
    }

    if (data->array_.size() == index) {
        data->array_.push_back(mgjson());
    }

    return data->array_.at(index);
}

mgjson
mgjson::at(const char* key) const
{
    const mgjson_private* data = d.data();

    data->check_key_is_empty(key);

    if (Object != data->type_) {
        return mgjson();
    }

    auto it = data->map_.find(*reinterpret_cast<const mgjson_private::Key*>(&key));
    if (data->map_.end() == it) {
        return mgjson();
    }
    return it->second;
}

mgjson&
mgjson::at(const char* key)
{
    mgjson_private* data = d.data();

    data->check_key_is_empty(key);

    switch(data->type_) {
    case Undefined:
    case Null:
    case Object:
        break;
    default:
        throw std::invalid_argument("mgjson::at(key) can't be used for json what is not an object.");
    }
    data->type_ = Object;
    data->array_.clear();
    return data->map_[mgjson_private::Key(key)];
}

#ifdef QT_CORE_LIB
QByteArrayList
mgjson::keys() const
{
    const mgjson_private* data = d.data();
    QByteArrayList res;
    if (Object == data->type_) {
        res.reserve(static_cast<int>(data->map_.size()));
        for (const auto it : data->map_) {
            res.push_back(it.first.d);
        }
    }
    return res;
}
#else
std::vector<std::string>
mgjson::keys() const
{
    const mgjson_private* data = d.data();
    std::vector<std::string> res;
    if (Object == data->type_) {
        res.reserve(data->map_.size());
        for (const auto it : data->map_) {
            res.push_back(it.first.d);
        }
    }
    return res;
}
#endif

mgjson&
mgjson::push_back(const mgjson& value)
{
    mgjson_private* data = d.data();
    if (!data->switch_to_array()) {
        throw std::invalid_argument("mgjson::push_back can't be used for json what is not an array.");
    }

    data->array_.push_back(value);
    return data->array_.back();
}

mgjson&
mgjson::push_front(const mgjson& value)
{
    mgjson_private* data = d.data();
    if (!data->switch_to_array()) {
        throw std::invalid_argument("mgjson::push_front can't be used for json what is not an array.");
    }

    return *(data->array_.insert(data->array_.begin(), value));
}

void
mgjson::remove(size_t index)
{
    mgjson_private* data = d.data();
    if ((Array != data->type_) || (data->array_.size() <= index)) {
        return;
    }

    /// \todo Maybe, it makes sens to change erase to memmove
    /// to prevent construction/destruction calls
    data->array_.erase(data->array_.begin() + index);
}

void
mgjson::remove(const char* key)
{
    mgjson_private* data = d.data();
    if (Object != data->type_) {
        return;
    }
    data->map_.erase(*reinterpret_cast<const mgjson_private::Key*>(&key));
}
