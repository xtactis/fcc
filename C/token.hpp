#ifndef TOKEN_H
#define TOKEN_H

enum class TokenType { // NOTE(mdizdar): this is probably bad, use a regular enum... I guess
    // TODO(mdizdar): write all of the possible tokens here bro, there aren't that many bro
};

struct Token {
    TokenType type;
    std::string value;
    u32 symbolTableIndex;
};

struct Node {
    Token token;
    Node *left, *right;
}

#endif // TOKEN_H