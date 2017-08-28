#include "mgjson.h"

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

