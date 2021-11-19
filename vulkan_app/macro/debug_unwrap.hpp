#pragma once

#include "for_each.hpp"
#include "_/strfix.hpp"

#define NAME_COLON_VAL_COMMA_NL_ADD(v) #v + std::string(": ") + std::to_string(v) + ",\n" +
#define NV_LIST(...) (FOR_EACH(NAME_COLON_VAL_COMMA_NL_ADD, __VA_ARGS__) "")
#define MAKE_DEBUG_UNWRAP(...) std::string debug_unwrap() { return NV_LIST(__VA_ARGS__); }