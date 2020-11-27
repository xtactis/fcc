#include <cstdio>
#include <cstdlib>

#include <vector>
#include <string>
#include <map>
#include <algorithm>

#include "utils/common.hpp"

/************************ TOKEN ****************************/

enum TokenType {
  Unknown = 0,
  Identifier,
  Typename,
  OpenParen,
  CloseParen,
  OpenBrace,
  CloseBrace,
  Keyword,
  IntegerLiteral,
  Semicolon,

  // not really tokens but my code my rules
  Program,
  FunctionList,
  Function,
  Block,
  Statement,
  Expression
};

struct Token {
  TokenType type;
  std::string value;
  static const std::map<std::string, TokenType> typemap;

  Token() : type(TokenType::Unknown), value("") {}
  
  Token(const Token &tok) {
    type = tok.type;
    value = tok.value;
  }

  Token& operator=(const Token &other) {
    this->type = other.type;
    this->value = other.value;
    return *this;
  }
  
  Token(const std::string &tok) : value(tok) {
    if (typemap.count(tok)) {
      type = typemap.at(tok);
    } else if (isalpha(tok[0])) {
      type = TokenType::Identifier;
    } else if (isdigit(tok[0])) {
      type = TokenType::IntegerLiteral;
    } else {
      // TODO: where? which one?
      error("Unrecognized identifier");
    }
  }
  
  void print() const {
    const char *s = 0;
#define PROCESS_VAL(p) case(p): s = #p; break;
    switch (type) {
      PROCESS_VAL(Unknown);
      PROCESS_VAL(Identifier);
      PROCESS_VAL(Typename);
      PROCESS_VAL(OpenParen);
      PROCESS_VAL(CloseParen);
      PROCESS_VAL(OpenBrace);
      PROCESS_VAL(CloseBrace);
      PROCESS_VAL(Keyword);
      PROCESS_VAL(IntegerLiteral);
      PROCESS_VAL(Semicolon);
      
      PROCESS_VAL(Program);
      PROCESS_VAL(FunctionList);
      PROCESS_VAL(Function);
      PROCESS_VAL(Block);
      PROCESS_VAL(Statement);
      PROCESS_VAL(Expression);
    }
#undef PROCESS_VAL
    printf("{%s, '%s'}", s, value.c_str());
  }
};
const std::map<std::string, TokenType> Token::typemap =
  {
   {"(", TokenType::OpenParen},
   {")", TokenType::CloseParen},
   {"{", TokenType::OpenBrace},
   {"}", TokenType::CloseBrace},
   {";", TokenType::Semicolon},
   {"Program", TokenType::Program},
   {"FunctionList", TokenType::FunctionList},
   {"Function", TokenType::Function},
   {"Block", TokenType::Block},
   {"Statement", TokenType::Statement},
   {"Expression", TokenType::Expression}
  };

std::vector<Token> tokenize(const std::string &code) {
  std::vector<Token> tokens;
  std::string tok;
  for (size_t i = 0; i < code.size(); ++i) {
    const char &c = code[i];
    if (isspace(c)) {
      if (tok.size()) tokens.emplace_back(tok);
      tok.clear();
      continue;
    }
    if (tok.empty()) {
      tok += c;
      continue;
    }
    if (isdigit(c)) {
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
	tokens.emplace_back(tok);
	tok.clear();
	tok += c;
      }
    } else {
      // TODO: add support for multicharacter operators
      // and non-alnum identifiers (or w/e)
      tokens.emplace_back(tok);
      tok.clear();
      tok += c;
    }
  }
  if (tok.size()) tokens.emplace_back(tok);
  return tokens;
}

/************************* AST ****************************/

struct AST {
  Token token;
  std::vector<AST> ast;

  AST(const AST &ast) : token(ast.token), ast(ast.ast) { }
  AST(const std::string &_token) : token(_token) { }
  AST(const Token &_token) : token(_token) { }

  void add(const std::string &token) {
    ast.emplace_back(token);
  }

  void add(const Token &token) {
    ast.emplace_back(token);
  }
  
  void add(const std::vector<AST> &tokens) {
    for (const auto &t: tokens) {
      this->ast.emplace_back(t);
    }
  }
  
  void print(std::string indent = "") const {
    printf(indent.c_str());
    token.print();
    puts("");
    for (const auto &e: ast) {
      e.print(indent+"  ");
    }
  }
};

/*
const std::vector<TokenType> programStructure =
  {
   TokenType::FunctionList
  };
*/



const std::vector<TokenType> functionStructure =
  {
   TokenType::Typename,
   TokenType::Identifier,
   TokenType::OpenParen,
   TokenType::CloseParen,
   TokenType::OpenBrace,
   TokenType::Block,
   TokenType::CloseBrace
  };

const std::vector<TokenType> statementStructure =
  {
   TokenType::Keyword,
   TokenType::Expression,
   TokenType::Semicolon
  };

const std::vector<TokenType> expressionStructure =
  {
   TokenType::IntegerLiteral
  };

const std::vector<TokenType> programStructure = statementStructure;

std::vector<AST> parse(std::vector<Token> &tokens, const std::vector<TokenType> &structure) {
  std::vector<AST> ret;
  int ind = tokens.size();
  for (TokenType t: structure) {
    switch (t) {
      case TokenType::FunctionList: {
	ret.emplace_back("FunctionList");
	while (tokens.size()) {
	  ret.back().add(parse(tokens, functionStructure));
	}
	break;
      }
      case TokenType::Block: {
	ret.emplace_back("Block");
	while (tokens.back().type != TokenType::CloseBrace) {
	  ret.back().add(parse(tokens, statementStructure));
	}
	break;
      }
      case TokenType::Expression: {
	ret.emplace_back("Expression");
	ret.back().add(parse(tokens, expressionStructure));
	break;
      }
      default: {
	if (tokens.back().type == TokenType::Identifier) {
	  if (t == TokenType::Typename) {
	    tokens.back().type = t;
	  } else if (t == TokenType::Keyword) {
	    tokens.back().type = t;
	  }
	}
	if (t != tokens.back().type) {
	  printf("%d %d\n", t, tokens.back().type);
	  tokens.back().print();
	  error("Something wrong bro");
	}
	ret.emplace_back(tokens.back());
	tokens.pop_back();
      }
    }
  }
  return ret;
}

void validate(const AST &ast) {
  //NOT_IMPL
  warning("AST validation not implemented");
}

/************************* IR ****************************/

struct IR {
  short bytecode;
  unsigned long long value;
  
  void print() const {
    char *s = 0;
    if (bytecode == 0) {
      sprintf(s, "nop");
    } else if (bytecode == 1) {
      sprintf(s, "ret (r0)");
    } else if (bytecode == 2) {
      sprintf(s, "jmp %lld", value);
    } else {
      error("Unrecognized bytecode.\n How did you even do this?");
    }
    puts(s);
  }
};

std::vector<IR> generateIR(const AST &ast) {
  NOT_IMPL
}

/************************ MAIN ****************************/

int main(int argc, char **argv) {
  if (false) {
    if (argc == 1) {
      error("No arguments provided");
    }
  }
  puts(CYAN "***CODE***" RESET);
  const std::string code =
    //"int main() {\n"
    "  return 42;\n";
    //"}             ";
  puts(code.c_str());
  puts("");
  puts(CYAN "**TOKENS**" RESET);
  std::vector<Token> tokens = tokenize(code);
  for (const auto &e: tokens) {
    e.print();
    puts("");
  }
  puts("");
  puts(CYAN "****AST***" RESET);
  std::reverse(tokens.begin(), tokens.end());
  AST ast("Program");
  ast.add(parse(tokens, programStructure));
  ast.print();
  puts("");
  validate(ast);
  puts(CYAN "****IR*****" RESET);
  const std::vector<IR> ir = generateIR(ast);
  for (const auto &e: ir) {
    e.print();
  }
}
