#ifndef STRUCT_TYPE_H
#define STRUCT_TYPE_H

#include "../utils/common.h"
#include "declaration.h"

STRUCT_HEADER(StructType, {
    DeclarationArray members;
    //SymbolTable member_table;
});


#endif //STRUCT_TYPE_H
