#ifndef LINE_TID_H
#define LINE_TID_H

#include "../utils/common.h"

typedef u64 TemporaryID;
typedef u64 Line;

STRUCT_HEADER(LineTemporaryID, {
    TemporaryID id;
    Line line;
});

#endif //LINE_TID_H
