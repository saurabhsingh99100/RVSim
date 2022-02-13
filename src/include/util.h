#pragma once

#define BOOLSTRING(x) \
    x ? "true" : "false"

#ifdef DBG_CODE
#   define DBG(x) x
#else
#   define DBG(x)
#endif


void throwError(std::string msg, bool exit);