#ifndef CACHED_TOKEN_H
#define CACHED_TOKEN_H

#include "../utils/common.h"
#include "token.h"

STRUCT_HEADER(CachedToken, {
    Token *token;
    u64 lookahead;
    u64 cur_line;
    u64 cur_col;
});

#endif //CACHED_TOKEN_H
