#include <string>
#include <cstring>
#include <map>
#include <vector>
#include <limits>

#include "mgjson.h"

#ifndef QT_CORE_LIB
char *qstrdup(const char *);
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
      d_value_(value),
      str_value_(std::to_string(value))
    {
    }

public:
    struct Key {
        explicit Key(const Key& other) :
            d(qstrdup(other.d))
        {

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
    std::string str_value_;
    std::vector<mgjson> array_;
    std::map<Key,mgjson> map_;
};

mgjson::~mgjson()
{
}

mgjson::mgjson(mgjson_private *data) :
    d(data)
{
}

mgjson::mgjson(const mgjson& other) :
    d(other.d)
{
}

mgjson &mgjson::operator=(const mgjson& other)
{
  if( this != &other ) {
    d.operator=(other.d);
  }
  return *this;
}

mgjson::mgjson(json_type type) :
    d(new mgjson_private(type))
{
}

mgjson::mgjson(bool value) :
    d(new mgjson_private(value))
{
}

mgjson::mgjson(int value) :
    d(new mgjson_private(static_cast<unsigned long long>(value)))
{
}

mgjson::mgjson(unsigned int value) :
    d(new mgjson_private(static_cast<unsigned long long>(value)))
{
}

mgjson::mgjson(long value) :
    d(new mgjson_private(static_cast<unsigned long long>(value)))
{
}

mgjson::mgjson(unsigned long value) :
    d(new mgjson_private(static_cast<unsigned long long>(value)))
{
}

mgjson::mgjson(long long value) :
    d(new mgjson_private(static_cast<unsigned long long>(value)))
{
}

mgjson::mgjson(unsigned long long value) :
    d(new mgjson_private(value))
{
}

mgjson::json_type mgjson::type() const
{
  return d->type_;
}

