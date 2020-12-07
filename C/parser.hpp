#ifndef PARSER_H
#define PARSER_H

#include <cstdlib>

#include <vector>
#include <string>

#include "../utils/common.hpp"
#include "token.hpp"

#define parse_error error("Parse error!")
#define roll_back { code.peek = code.ptr; return 0; }
#define confirm_peek { code.ptr = code.peek; return 1; }

struct SymbolTableEntry {
    
};

namespace CParser {
    struct Code {
        u64 ptr = 0;
        u64 peek = 0;
        // NOTE(mdizdar): I'm not sure peek should be contained in the Code struct; or at least not used how it's used currently
        // I have a hunch it should be a function local variable so the roll back doesn't go back too far
        // TODO(mdizdar): actual code, and other stuff
    };
    
    void jumpStatement(Code &code) {
        /*
<jump-statement> ::= goto <identifier> ;
                   | continue ;
                   | break ;
                   | return {<expression>}? ;
*/
    }
    
    void iterationStatement(Code &code) {
        /*
<iteration-statement> ::= while ( <expression> ) <statement>
                        | do <statement> while ( <expression> ) ;
                        | for ( {<expression>}? ; {<expression>}? ; {<expression>}? ) <statement>
*/
    }
    
    void selectionStatement(Code &code) {
        /*
<selection-statement> ::= if ( <expression> ) <statement>
                        | if ( <expression> ) <statement> else <statement>
                        | switch ( <expression> ) <statement>
*/
    }
    
    void expressionStatement(Code &code) {
        /*
<expression-statement> ::= {<expression>}? ;
*/
    }
    
    void labeledStatement(Code &code) {
        /*
<labeled-statement> ::= <identifier> : <statement>
                      | case <constant-expression> : <statement>
                      | default : <statement>
*/
    }
    
    void statement(Code &code) {
        /*
<statement> ::= <labeled-statement>
              | <expression-statement>
              | <compound-statement>
              | <selection-statement>
              | <iteration-statement>
              | <jump-statement>
*/
    }
    
    void initializerList(Code &code) {
        /*
<initializer-list> ::= <initializer>
                     | <initializer-list> , <initializer>
*/
    }
    
    void initializer(Code &code) {
        /*
<initializer> ::= <assignment-expression>
                | { <initializer-list> }
                | { <initializer-list> , }
*/
    }
    
    void initDeclarator(Code &code) {
        /*
<init-declarator> ::= <declarator>
                    | <declarator> = <initializer>
*/
    }
    
    void enumerator(Code &code) {
        /*
<enumerator> ::= <identifier>
               | <identifier> = <constant-expression>
*/
    }
    
    void enumeratorList(Code &code) {
        /*
<enumerator-list> ::= <enumerator>
                    | <enumerator-list> , <enumerator>
*/
    }
    
    void directAbstractDeclarator(Code &code) {
        /*
<direct-abstract-declarator> ::=  ( <abstract-declarator> )
                               | {<direct-abstract-declarator>}? [ {<constant-expression>}? ]
                               | {<direct-abstract-declarator>}? ( {<parameter-type-list>}? )
*/
    }
    
    void parameterDeclaration(Code &code) {
        /*
<parameter-declaration> ::= {<declaration-specifier>}+ <declarator>
                          | {<declaration-specifier>}+ <abstract-declarator>
                          | {<declaration-specifier>}+
*/
    }
    
    void parameterList(Code &code) {
        /*
<parameter-list> ::= <parameter-declaration>
                   | <parameter-list> , <parameter-declaration>
*/
    }
    
    void abstractDeclarator(Code &code) {
        /*

<abstract-declarator> ::= <pointer>
                        | <pointer> <direct-abstract-declarator>
                        | <direct-abstract-declarator>
*/
    }
    
    void assignmentOperator(Code &code) {
        /*
        <assignment-operator> ::= =
            | *=
            | /=
            | %=
            | +=
            | -=
            | <<=
            | >>=
            | &=
            | ^=
            | |=
            */
    }
    
    void enumerationConstant(Code &code) {
        // TODO(mdizdar): what's an enumeration constant lol
    }
    
    void floatingConstant(Code &code) {
        // TODO(mdizdar): what's a floating constant lol
    }
    
    void characterConstant(Code &code) {
        // TODO(mdizdar): what's a character constant lol
    }
    
    void integerConstant(Code &code) {
        // TODO(mdizdar): what's an integer constant lol
    }
    
    void string(Code &code) {
        // TODO(mdizdar): what's a string lol
    }
    
    void constant(Code &code) {
        /*
<constant> ::= <integer-constant>
             | <character-constant>
             | <floating-constant>
             | <enumeration-constant>
*/
    }
    
    void assignmentExpression(Code &code) {
        /*
<assignment-expression> ::= <conditional-expression>
                          | <unary-expression> <assignment-operator> <assignment-expression>
*/
    }
    
    void primaryExpression(Code &code) {
        /*
<primary-expression> ::= <identifier>
                       | <constant>
                       | <string>
                       | ( <expression> )
*/
    }
    
    void postfixExpression(Code &code) {
        /*
<postfix-expression> ::= <primary-expression>
                       | <postfix-expression> [ <expression> ]
                       | <postfix-expression> ( {<assignment-expression>}* )
                       | <postfix-expression> . <identifier>
                       | <postfix-expression> -> <identifier>
                       | <postfix-expression> ++
                       | <postfix-expression> --
*/
    }
    
    void unaryOperator(Code &code) {
        /*
<unary-operator> ::= &
                   | *
                   | +
                   | -
                   | ~
                   | !
*/
    }
    
    void typeName(Code &code) {
        /*
<type-name> ::= {<specifier-qualifier>}+ {<abstract-declarator>}?
*/
    }
    
    void unaryExpression(Code &code) {
        /*
<unary-expression> ::= <postfix-expression>
                     | ++ <unary-expression>
                     | -- <unary-expression>
                     | <unary-operator> <cast-expression>
                     | sizeof <unary-expression>
                     | sizeof <type-name>
*/
    }
    
    void castExpression(Code &code) {
        /*
<cast-expression> ::= <unary-expression>
                    | ( <type-name> ) <cast-expression>
*/
    }
    
    void multiplicativeExpression(Code &code) {
        /*
<multiplicative-expression> ::= <cast-expression>
                              | <multiplicative-expression> * <cast-expression>
                              | <multiplicative-expression> / <cast-expression>
                              | <multiplicative-expression> % <cast-expression>
*/
    }
    
    void additiveExpression(Code &code) {
        /*
<additive-expression> ::= <multiplicative-expression>
                        | <additive-expression> + <multiplicative-expression>
                        | <additive-expression> - <multiplicative-expression>
*/
    }
    
    void shiftExpression(Code &code) {
        /*
<shift-expression> ::= <additive-expression>
                     | <shift-expression> << <additive-expression>
                     | <shift-expression> >> <additive-expression>
*/
    }
    
    void relationalExpression(Code &code) {
        /*
<relational-expression> ::= <shift-expression>
                          | <relational-expression> < <shift-expression>
                          | <relational-expression> > <shift-expression>
                          | <relational-expression> <= <shift-expression>
                          | <relational-expression> >= <shift-expression>
*/
    }
    
    void equalityExpression(Code &code) {
        /*
<equality-expression> ::= <relational-expression>
                        | <equality-expression> == <relational-expression>
                        | <equality-expression> != <relational-expression>
*/
    }
    
    void andExpression(Code &code) {
        /*
<and-expression> ::= <equality-expression>
                   | <and-expression> & <equality-expression>
*/
        
    }
    
    void exclusiveOrExpression(Code &code) {
        /*
<exclusive-or-expression> ::= <and-expression>
                            | <exclusive-or-expression> ^ <and-expression>
*/
    }
    
    void inclusiveOrExpression(Code &code) {
        /*
<inclusive-or-expression> ::= <exclusive-or-expression>
                            | <inclusive-or-expression> | <exclusive-or-expression>
*/
    }
    
    void logicalAndExpression(Code &code) {
        /*
<logical-and-expression> ::= <inclusive-or-expression>
                           | <logical-and-expression> && <inclusive-or-expression>
*/
    }
    
    void expression(Code &code) {
        /*
<expression> ::= <assignment-expression>
               | <expression> , <assignment-expression>
*/
    }
    
    void logicalOrExpression(Code &code) {
        /*
<logical-or-expression> ::= <logical-and-expression>
                          | <logical-or-expression> || <logical-and-expression>
*/
    }
    
    void conditionalExpression(Code &code) {
        /*
<conditional-expression> ::= <logical-or-expression>
                           | <logical-or-expression> ? <expression> : <conditional-expression>
*/
    }
    
    void parameterTypeList(Code &code) {
        /*
<parameter-type-list> ::= <parameter-list>
            | <parameter-list> , ...
*/
    }
    
    void identifier(Code &code) {
        // TODO(mdizdar): what's an identifier lol
    }
    
    void directDeclarator(Code &code) {
        /*
<direct-declarator> ::= <identifier>
                      | ( <declarator> )
                      | <direct-declarator> [ {<constant-expression>}? ]
                      | <direct-declarator> ( <parameter-type-list> )
                      | <direct-declarator> ( {<identifier>}* )
*/
    }
    
    void pointer(Code &code) {
        /*
<pointer> ::= * {<type-qualifier>}* {<pointer>}?
*/
    }
    
    void constantExpression(Code &code) {
        /*
<constant-expression> ::= <conditional-expression>
*/
    }
    
    void structDeclarator(Code &code) {
        /*
<struct-declarator> ::= <declarator>
                      | <declarator> : <constant-expression>
                      | : <constant-expression>
*/
    }
    
    void structDeclaratorList(Code &code) {
        /*
<struct-declarator-list> ::= <struct-declarator>
                           | <struct-declarator-list> , <struct-declarator>
*/
    }
    
    void specifierQualifier(Code &code) {
        /*
<specifier-qualifier> ::= <type-specifier>
                        | <type-qualifier>
*/
    }
    
    void structDeclaration(Code &code) {
        /*
<struct-declaration> ::= {<specifier-qualifier>}* <struct-declarator-list>
*/
    }
    
    void structOrUnion(Code &code) {
        /*
<struct-or-union> ::= struct
                    | union
*/
    }
    
    void typedefName(Code &code) {
        /*
<typedef-name> ::= <identifier>
*/
    }
    
    void enumSpecifier(Code &code) {
        /*
<enum-specifier> ::= enum <identifier> { <enumerator-list> }
                   | enum { <enumerator-list> }
                   | enum <identifier>
*/
    }
    
    void structOrUnionSpecifier(Code &code) {
        /*
<struct-or-union-specifier> ::= <struct-or-union> <identifier> { {<struct-declaration>}+ }
                              | <struct-or-union> { {<struct-declaration>}+ }
                              | <struct-or-union> <identifier>
*/
    }
    
    void typeQualifier(Code &code) {
        /*
<type-qualifier> ::= const
                   | volatile
*/
        return match("const", code) | match("volatile", code);
    }
    
    u8 typeSpecifier(Code &code) {
        /*
<type-specifier> ::= void
                   | char
                   | short
                   | int
                   | long
                   | float
                   | double
                   | signed
                   | unsigned
                   | <struct-or-union-specifier>
                   | <enum-specifier>
                   | <typedef-name>
*/
        return match("void", code) | match("char", code) | match("short", code) | match("int", code) | 
            match("long", code) | match("float", code) | match("double", code) | match("signed", code) | 
            match("unsigned", code) | structOrUnionSpecifier(code) | enumSpecifier(code) | typedefName(code);
    }
    
    u8 storageClassSpecifier(Code &code) {
        /*
<storage-class-specifier> ::= auto
                            | register
                            | static
                            | extern
                            | typedef
*/
        return match("auto", code) | match("register", code) | match("static", code) | match("extern", code) | match("typedef", code);
    }
    
    u8 compoundStatement(Code &code) {
        /*
<compound-statement> ::= { {<declaration>}* {<statement>}* }
*/
        u8 retval = match("{", code);
        for (size_t code.ptr = 0; code.ptr < code.length;) {
            if (!declaration(code)) break;
        }
        for (size_t code.ptr = 0; code.ptr < code.length;) {
            if (!statement(code)) break;
        }
        return retval && match("}", code)
    }
    
    u8 declarator(Code &code) {
        /*
<declarator> ::= {<pointer>}? <direct-declarator>
*/
        if (!pointer(code)) code;
        return directDeclarator(code);
    }
    
    u8 declarationSpecifier(Code &code) {
        /*
<declaration-specifier> ::= <storage-class-specifier>
                          | <type-specifier>
                          | <type-qualifier>
*/
        if (storageClassSpecifier(code)) confirm_peek;
        code.peek = code.ptr;
        if (typeSpecifier(code)) confirm_peek;
        code.peek = code.ptr;
        if (typeQualifier(code)) confirm_peek;
        roll_back;
    }
    
    u8 functionDefinition(Code &code) {
        /*
<function-definition> ::= {<declaration-specifier>}* <declarator> {<declaration>}* <compound-statement>
*/
        for (size_t code.ptr = 0; code.ptr < code.length;) {
            if (!declarationSpecifier(code)) break;
        }
        if (!declarator(code)) roll_back;
        for (size_t code.ptr = 0; code.ptr < code.length;) {
            if (!declaration(code)) break;
        }
        if (!compoundStatement(code)) roll_back;
        confirm_peek;
    }
    
    u8 declaration(Code &code) {
        /*
<declaration> ::=  {<declaration-specifier>}+ {<init-declarator>}* ;
*/
        u8 retval = 0;
        for (size_t code.ptr = 0; code.ptr < code.length;) {
            u8 cur = declarationSpecifier(code);
            if (!cur) break;
            retval |= cur;
        }
        if (!retval) roll_back;
        for (size_t code.ptr = 0; code.ptr < code.length;) {
            if (!initDeclarator(code)) break;
        }
        if (!match(";", code)) roll_back;
        confirm_peek;
    }
    
    u8 externalDeclaration(Code &code) {
        /*
<external-declaration> ::= <function-definition>
                         | <declaration>
*/
        if (functionDefinition(code)) confirm_peek;
        code.peek = code.ptr;
        if (declaration(code)) confirm_peek;
        roll_back;
    }
    
    void parse(Code &code) {
        /*
<translation-unit> ::= {<external-declaration>}*
*/
        for (size_t code.ptr = 0; code.ptr < code.length;) {
            if (!externalDeclaration(code)) parse_error;
        }
    }
    
    Token createToken(const std::string &tok) {
        
    }
    
    std::vector<Token> tokenize(const std::string &code) {
        std::vector<Token> tokens;
        std::string tok;
        for (size_t i = 0; i < code.size(); ++i) {
            const char &c = code[i];
            if (isspace(c)) {
                if (tok.size()) {
                    tokens.push_back(createToken(tok));
                }
                tok.clear();
                continue;
            }
            if (tok.empty()) {
                tok += c;
                continue;
            }
            if (isdigit(c)) { // TODO(mdizdar): add support for hex numbers
                if (isalnum(tok[0])) {
                    tok += c;
                } else {
                    tokens.emplace_back(tok);
                    tok.clear();
                    tok += c;
                }
            } else if (isalpha(c) || c == '_') {
                if (isalpha(tok[0])) {
                    tok += c;
                } else {
                    tokens.push_back(createToken(tok));
                    tok.clear();
                    tok += c;
                }
            } else {
                // TODO(mdizdar): add support for multicharacter operators and non-alnum identifiers (or w/e)
                tokens.push_back(createToken(tok));
                tok.clear();
                tok += c;
            }
        }
        if (tok.size()) {
            tokens.push_back(tok);
        }
        return tokens;
    }
};

#endif // PARSER_H