#ifndef MPL_DETAIL_STATE_H
#define MPL_DETAIL_STATE_H

#include <map>
#include <vector>
#include <cstdint>

namespace mpl {
namespace detail {

typedef uint8_t Byte;

const Byte EPSILON = 0x0;
const Byte OTHER = 0xFF;

typedef std::vector<int> StateList;

typedef std::map<Byte, StateList> NFATran;

typedef std::map<Byte, int> DFATran;

typedef std::vector<int> Tag;
const Tag NONE_TAG;

} // namespace detail
} // namespace mpl

#endif // MPL_DETAIL_STATE_H