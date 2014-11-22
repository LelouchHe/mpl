#ifndef MPL_LEXER_DETAIL_STATE_H
#define MPL_LEXER_DETAIL_STATE_H

#include <map>
#include <set>
#include <vector>
#include <cstdint>

namespace mpl {
namespace lexer {
namespace detail {

typedef uint8_t Byte;

const Byte EPSILON = 0x0;
const Byte OTHER = 0xFF;

typedef std::set<int> States;

typedef std::map<Byte, States> NFATran;

typedef std::map<Byte, int> DFATran;

typedef std::set<int> Tag;
extern const Tag NONE_TAG;

} // namespace detail
} // namespace lexer
} // namespace mpl

#endif // MPL_LEXER_DETAIL_STATE_H