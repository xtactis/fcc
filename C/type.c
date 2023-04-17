#include "type.h"

STRUCT_SOURCE(Type);

// NOTE(mdizdar): in a real compiler this should depend on the target machine, but since we're targetting exactly one machine... 
u64 Type_sizeof(Type *type) {
    if (type->pointer_count) {
        return 2;
    } else if (type->is_struct) {
        u64 size = 0;
        for (u64 i = 0; i < type->struct_type->members.count; ++i) {
            u64 cur = Type_sizeof(((Declaration *)type->struct_type->members.data)[i].type);
            size += cur;
            size += size%min(cur, 8);
        }
        return size;
    } else if (type->is_union) {
        u64 size = 0;
        for (u64 i = 0; i < type->struct_type->members.count; ++i) {
            max(size, Type_sizeof(((Declaration *)type->struct_type->members.data)[i].type));
        }
        return size;
    } else if (type->is_typedef) {
        return Type_sizeof(type->typedef_type);
    } else if (type->is_array) {
        return type->array_type->size * Type_sizeof(type->array_type->element);
    } else if (type->is_function) {
        // should never happen
        internal_error;
    } else switch (type->basic_type) { // it's a basic type
        case BASIC_CHAR:
        case BASIC_SCHAR:
        case BASIC_UCHAR: return 1;
        case BASIC_SSHORT:
        case BASIC_USHORT:
        case BASIC_SINT:
        case BASIC_UINT: return 2;
        case BASIC_SLONG:
        case BASIC_ULONG:
        case BASIC_FLOAT:
        case BASIC_DOUBLE:
        case BASIC_LDOUBLE: return 4;
        case BASIC_SLLONG:
        case BASIC_ULLONG: return 8;
        case BASIC_VOID: return 1; // while techincally correct, I don't like it
        default: error(0, "sir, this is a wendy's");
    }
}

char *Type_toStr(char *s, const Type *type, bool in_line, u64 indent) {
    (void)in_line;
    (void)indent;
    if (type->is_union || type->is_struct || type->is_typedef || type->is_array || type->is_function) {
        // TODO(mdizdar): finish this
        return "<weird type>";
    }
    switch(type->basic_type) {
        case BASIC_ERROR: sprintf(s, "ERROR type"); break;
        case BASIC_VOID:  sprintf(s, "void"); break;
        case BASIC_CHAR:  sprintf(s, "char"); break;
        
        case BASIC_SCHAR:  sprintf(s, "signed char"); break;
        case BASIC_SSHORT: sprintf(s, "short int"); break;
        case BASIC_SINT:   sprintf(s, "int"); break;
        case BASIC_SLONG:  sprintf(s, "long int"); break;
        case BASIC_SLLONG: sprintf(s, "long long int"); break;
        
        case BASIC_FLOAT:   sprintf(s, "float"); break;
        case BASIC_DOUBLE:  sprintf(s, "double"); break;
        case BASIC_LDOUBLE: sprintf(s, "long double"); break;
        
        case BASIC_UCHAR:  sprintf(s, "unsigned char"); break;
        case BASIC_USHORT: sprintf(s, "unsigned short int"); break;
        case BASIC_UINT:   sprintf(s, "unsigned int"); break;
        case BASIC_ULONG:  sprintf(s, "unsigned long int"); break;
        case BASIC_ULLONG: sprintf(s, "unsigned long long int"); break;
    }
    return s;
}

void Type_print(Type *type, u64 indent) {
    for (u64 i = 0; i < indent; ++i) putchar(' ');
    if (type->is_static) printf("static ");
    if (Bitset_isSet(type->is_const, 0)) printf("const ");
    if (Bitset_isSet(type->is_volatile, 0)) printf("volatile ");
    if (Bitset_isSet(type->is_restrict, 0)) printf("restrict ");
    if (type->is_struct) {
        printf("struct { [%lu] \n", type->struct_type->members.count);
        for (u64 i = 0; i < type->struct_type->members.count; ++i) {
            Type_print(((Declaration*)type->struct_type->members.data)[i].type, indent+2);
            
        }
        for (u64 i = 0; i < indent; ++i) putchar(' ');
        printf("}");
    } else if (type->is_union) {
        // TODO(mdizdar)
    } else if (type->is_typedef) {
        // TODO(mdizdar)
    } else if (type->is_array) {
        // TODO(mdizdar)
    } else if (type->is_function) {
        // TODO(mdizdar)
    } else {
        switch(type->basic_type) {
            case BASIC_ERROR: printf("ERROR type "); break;
            case BASIC_VOID:  printf("void "); break;
            case BASIC_CHAR:  printf("char "); break;
            
            case BASIC_SCHAR:  printf("signed char "); break;
            case BASIC_SSHORT: printf("short int "); break;
            case BASIC_SINT:   printf("int "); break;
            case BASIC_SLONG:  printf("long int "); break;
            case BASIC_SLLONG: printf("long long int "); break;
            
            case BASIC_FLOAT:   printf("float "); break;
            case BASIC_DOUBLE:  printf("double "); break;
            case BASIC_LDOUBLE: printf("long double "); break;
            
            case BASIC_UCHAR:  printf("unsigned char "); break;
            case BASIC_USHORT: printf("unsigned short int "); break;
            case BASIC_UINT:   printf("unsigned int "); break;
            case BASIC_ULONG:  printf("unsigned long int "); break;
            case BASIC_ULLONG: printf("unsigned long long int "); break;
        }
    }
    
    for (u64 i = 1; i <= type->pointer_count; ++i) {
        printf("* ");
        if (Bitset_isSet(type->is_const, i)) printf("const ");
        if (Bitset_isSet(type->is_volatile, i)) printf("volatile ");
        if (Bitset_isSet(type->is_restrict, i)) printf("restrict ");
    }
    puts("");
}

//~ some globals to keep the cost of type-making lower

// TODO(mdizdar): these should be set in main (because MSVC doesn't have struct literals, since it's 1993) so these common types can be reused
// NOTE(mdizdar); if these are ever going to become a thing, they'll need to be declared extern in any file that uses them and they'll need to be consts
/*
Type *TYPE_VOID;
Type *TYPE_CHAR;

Type *TYPE_SCHAR;
Type *TYPE_SSHORT;
Type *TYPE_SINT;
Type *TYPE_SLONG;
Type *TYPE_SLLONG;

Type *TYPE_FLOAT;
Type *TYPE_DOUBLE;
Type *TYPE_LDOUBLE;

Type *TYPE_UCHAR;
Type *TYPE_USHORT;
Type *TYPE_UINT;
Type *TYPE_ULONG;
Type *TYPE_ULLONG;
*/
