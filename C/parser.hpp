#ifndef PARSER_H
#define PARSER_H

#include <cstdlib>

#include <vector>
#include <string>

#include "../utils/common.hpp"
#include "token.hpp"

struct SymbolTableEntry {
    
};

namespace CParser {
    void jumpStatement(std::vector<Token> &tokens) {
        /*
<jump-statement> ::= goto <identifier> ;
                   | continue ;
                   | break ;
                   | return {<expression>}? ;
*/
    }
    
    void iterationStatement(std::vector<Token> &tokens) {
        /*
<iteration-statement> ::= while ( <expression> ) <statement>
                        | do <statement> while ( <expression> ) ;
                        | for ( {<expression>}? ; {<expression>}? ; {<expression>}? ) <statement>
*/
    }
    
    void selectionStatement(std::vector<Token> &tokens) {
        /*
<selection-statement> ::= if ( <expression> ) <statement>
                        | if ( <expression> ) <statement> else <statement>
                        | switch ( <expression> ) <statement>
*/
    }
    
    void expressionStatement(std::vector<Token> &tokens) {
        /*
<expression-statement> ::= {<expression>}? ;
*/
    }
    
    void labeledStatement(std::vector<Token> &tokens) {
        /*
<labeled-statement> ::= <identifier> : <statement>
                      | case <constant-expression> : <statement>
                      | default : <statement>
*/
    }
    
    void statement(std::vector<Token> &tokens) {
        /*
<statement> ::= <labeled-statement>
              | <expression-statement>
              | <compound-statement>
              | <selection-statement>
              | <iteration-statement>
              | <jump-statement>
*/
    }
    
    void initializerList(std::vector<Token> &tokens) {
        /*
<initializer-list> ::= <initializer>
                     | <initializer-list> , <initializer>
*/
    }
    
    void initializer(std::vector<Token> &tokens) {
        /*
<initializer> ::= <assignment-expression>
                | { <initializer-list> }
                | { <initializer-list> , }
*/
    }
    
    void initDeclarator(std::vector<Token> &tokens) {
        /*
<init-declarator> ::= <declarator>
                    | <declarator> = <initializer>
*/
    }
    
    void enumerator(std::vector<Token> &tokens) {
        /*
<enumerator> ::= <identifier>
               | <identifier> = <constant-expression>
*/
    }
    
    void enumeratorList(std::vector<Token> &tokens) {
        /*
<enumerator-list> ::= <enumerator>
                    | <enumerator-list> , <enumerator>
*/
    }
    
    void directAbstractDeclarator(std::vector<Token> &tokens) {
        /*
<direct-abstract-declarator> ::=  ( <abstract-declarator> )
                               | {<direct-abstract-declarator>}? [ {<constant-expression>}? ]
                               | {<direct-abstract-declarator>}? ( {<parameter-type-list>}? )
*/
    }
    
    void parameterDeclaration(std::vector<Token> &tokens) {
        /*
<parameter-declaration> ::= {<declaration-specifier>}+ <declarator>
                          | {<declaration-specifier>}+ <abstract-declarator>
                          | {<declaration-specifier>}+
*/
    }
    
    void parameterList(std::vector<Token> &tokens) {
        /*
<parameter-list> ::= <parameter-declaration>
                   | <parameter-list> , <parameter-declaration>
*/
    }
    
    void abstractDeclarator(std::vector<Token> &tokens) {
        /*

<abstract-declarator> ::= <pointer>
                        | <pointer> <direct-abstract-declarator>
                        | <direct-abstract-declarator>
*/
    }
    
    void assignmentOperator(std::vector<Token> &tokens) {
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
    
    void enumerationConstant(std::vector<Token> &tokens) {
        // TODO(mdizdar): what's an enumeration constant lol
    }
    
    void floatingConstant(std::vector<Token> &tokens) {
        // TODO(mdizdar): what's a floating constant lol
    }
    
    void characterConstant(std::vector<Token> &tokens) {
        // TODO(mdizdar): what's a character constant lol
    }
    
    void integerConstant(std::vector<Token> &tokens) {
        // TODO(mdizdar): what's an integer constant lol
    }
    
    void string(std::vector<Token> &tokens) {
        // TODO(mdizdar): what's a string lol
    }
    
    void constant(std::vector<Token> &tokens) {
        /*
<constant> ::= <integer-constant>
             | <character-constant>
             | <floating-constant>
             | <enumeration-constant>
*/
    }
    
    void assignmentExpression(std::vector<Token> &tokens) {
        /*
<assignment-expression> ::= <conditional-expression>
                          | <unary-expression> <assignment-operator> <assignment-expression>
*/
    }
    
    void primaryExpression(std::vector<Token> &tokens) {
        /*
<primary-expression> ::= <identifier>
                       | <constant>
                       | <string>
                       | ( <expression> )
*/
    }
    
    void postfixExpression(std::vector<Token> &tokens) {
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
    
    void unaryOperator(std::vector<Token> &tokens) {
        /*
<unary-operator> ::= &
                   | *
                   | +
                   | -
                   | ~
                   | !
*/
    }
    
    void typeName(std::vector<Token> &tokens) {
        /*
<type-name> ::= {<specifier-qualifier>}+ {<abstract-declarator>}?
*/
    }
    
    void unaryExpression(std::vector<Token> &tokens) {
        /*
<unary-expression> ::= <postfix-expression>
                     | ++ <unary-expression>
                     | -- <unary-expression>
                     | <unary-operator> <cast-expression>
                     | sizeof <unary-expression>
                     | sizeof <type-name>
*/
    }
    
    void castExpression(std::vector<Token> &tokens) {
        /*
<cast-expression> ::= <unary-expression>
                    | ( <type-name> ) <cast-expression>
*/
    }
    
    void multiplicativeExpression(std::vector<Token> &tokens) {
        /*
<multiplicative-expression> ::= <cast-expression>
                              | <multiplicative-expression> * <cast-expression>
                              | <multiplicative-expression> / <cast-expression>
                              | <multiplicative-expression> % <cast-expression>
*/
    }
    
    void additiveExpression(std::vector<Token> &tokens) {
        /*
<additive-expression> ::= <multiplicative-expression>
                        | <additive-expression> + <multiplicative-expression>
                        | <additive-expression> - <multiplicative-expression>
*/
    }
    
    void shiftExpression(std::vector<Token> &tokens) {
        /*
<shift-expression> ::= <additive-expression>
                     | <shift-expression> << <additive-expression>
                     | <shift-expression> >> <additive-expression>
*/
    }
    
    void relationalExpression(std::vector<Token> &tokens) {
        /*
<relational-expression> ::= <shift-expression>
                          | <relational-expression> < <shift-expression>
                          | <relational-expression> > <shift-expression>
                          | <relational-expression> <= <shift-expression>
                          | <relational-expression> >= <shift-expression>
*/
    }
    
    void equalityExpression(std::vector<Token> &tokens) {
        /*
<equality-expression> ::= <relational-expression>
                        | <equality-expression> == <relational-expression>
                        | <equality-expression> != <relational-expression>
*/
    }
    
    void andExpression(std::vector<Token> &tokens) {
        /*
<and-expression> ::= <equality-expression>
                   | <and-expression> & <equality-expression>
*/
        
    }
    
    void exclusiveOrExpression(std::vector<Token> &tokens) {
        /*
<exclusive-or-expression> ::= <and-expression>
                            | <exclusive-or-expression> ^ <and-expression>
*/
    }
    
    void inclusiveOrExpression(std::vector<Token> &tokens) {
        /*
<inclusive-or-expression> ::= <exclusive-or-expression>
                            | <inclusive-or-expression> | <exclusive-or-expression>
*/
    }
    
    void logicalAndExpression(std::vector<Token> &tokens) {
        /*
<logical-and-expression> ::= <inclusive-or-expression>
                           | <logical-and-expression> && <inclusive-or-expression>
*/
    }
    
    void expression(std::vector<Token> &tokens) {
        /*
<expression> ::= <assignment-expression>
               | <expression> , <assignment-expression>
*/
    }
    
    void logicalOrExpression(std::vector<Token> &tokens) {
        /*
<logical-or-expression> ::= <logical-and-expression>
                          | <logical-or-expression> || <logical-and-expression>
*/
    }
    
    void conditionalExpression(std::vector<Token> &tokens) {
        /*
<conditional-expression> ::= <logical-or-expression>
                           | <logical-or-expression> ? <expression> : <conditional-expression>
*/
    }
    
    void parameterTypeList(std::vector<Token> &tokens) {
        /*
<parameter-type-list> ::= <parameter-list>
            | <parameter-list> , ...
*/
    }
    
    void identifier(std::vector<Token> &tokens) {
        // TODO(mdizdar): what's an identifier lol
    }
    
    void directDeclarator(std::vector<Token> &tokens) {
        /*
<direct-declarator> ::= <identifier>
                      | ( <declarator> )
                      | <direct-declarator> [ {<constant-expression>}? ]
                      | <direct-declarator> ( <parameter-type-list> )
                      | <direct-declarator> ( {<identifier>}* )
*/
    }
    
    void pointer(std::vector<Token> &tokens) {
        /*
<pointer> ::= * {<type-qualifier>}* {<pointer>}?
*/
    }
    
    void constantExpression(std::vector<Token> &tokens) {
        /*
<constant-expression> ::= <conditional-expression>
*/
    }
    
    void structDeclarator(std::vector<Token> &tokens) {
        /*
<struct-declarator> ::= <declarator>
                      | <declarator> : <constant-expression>
                      | : <constant-expression>
*/
    }
    
    void structDeclaratorList(std::vector<Token> &tokens) {
        /*
<struct-declarator-list> ::= <struct-declarator>
                           | <struct-declarator-list> , <struct-declarator>
*/
    }
    
    void specifierQualifier(std::vector<Token> &tokens) {
        /*
<specifier-qualifier> ::= <type-specifier>
                        | <type-qualifier>
*/
    }
    
    void structDeclaration(std::vector<Token> &tokens) {
        /*
<struct-declaration> ::= {<specifier-qualifier>}* <struct-declarator-list>
*/
    }
    
    void structOrUnion(std::vector<Token> &tokens) {
        /*
<struct-or-union> ::= struct
                    | union
*/
    }
    
    void typedefName(std::vector<Token> &tokens) {
        /*
<typedef-name> ::= <identifier>
*/
    }
    
    void enumSpecifier(std::vector<Token> &tokens) {
        /*
<enum-specifier> ::= enum <identifier> { <enumerator-list> }
                   | enum { <enumerator-list> }
                   | enum <identifier>
*/
    }
    
    void structOrUnionSpecifier(std::vector<Token> &tokens) {
        /*
<struct-or-union-specifier> ::= <struct-or-union> <identifier> { {<struct-declaration>}+ }
                              | <struct-or-union> { {<struct-declaration>}+ }
                              | <struct-or-union> <identifier>
*/
    }
    
    void typeQualifier(std::vector<Token> &tokens) {
        /*
<type-qualifier> ::= const
                   | volatile
*/
    }
    
    void typeSpecifier(std::vector<Token> &tokens) {
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
    }
    
    void storageClassSpecifier(std::vector<Token> &tokens) {
        /*
<storage-class-specifier> ::= auto
                            | register
                            | static
                            | extern
                            | typedef
*/
    }
    
    void compoundStatement(std::vector<Token> &tokens) {
        /*
<compound-statement> ::= { {<declaration>}* {<statement>}* }
*/
    }
    
    void declarator(std::vector<Token> &tokens) {
        /*
<declarator> ::= {<pointer>}? <direct-declarator>
*/
    }
    
    void declarationSpecifier(std::vector<Token> &tokens) {
        /*
<declaration-specifier> ::= <storage-class-specifier>
                          | <type-specifier>
                          | <type-qualifier>
*/
    }
    
    void functionDefinition(std::vector<Token> &tokens) {
        /*
<function-definition> ::= {<declaration-specifier>}* <declarator> {<declaration>}* <compound-statement>
*/
    }
    
    void declaration(std::vector<Token> &tokens) {
        /*
<declaration> ::=  {<declaration-specifier>}+ {<init-declarator>}* ;
*/
    }
    
    void externalDeclaration(std::vector<Token> &tokens) {
        /*
<external-declaration> ::= <function-definition>
                         | <declaration>
*/
    }
    
    void translationUnit(std::vector<Token> &tokens) {
        /*
<translation-unit> ::= {<external-declaration>}*
*/
    }
    
    void parse(std::vector<Token> &tokens) {
        
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