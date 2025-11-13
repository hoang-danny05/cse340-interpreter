/*
 * Copyright (C) Rida Bazzi, 2017
 *
 * Do not share this file with anyone
 */
#ifndef __LEXER__H__
#define __LEXER__H__

#include <vector>
#include <string>
#include <map>

#include "inputbuf.h"
#include "execute.h"

// ------- token types -------------------

typedef enum { END_OF_FILE = 0,
    VAR, FOR, IF, WHILE, SWITCH, CASE, DEFAULT, INPUT, OUTPUT, ARRAY,
    PLUS, MINUS, DIV, MULT,
    EQUAL, COLON, COMMA, SEMICOLON,
    LBRAC, RBRAC, LPAREN, RPAREN, LBRACE, RBRACE,
    NOTEQUAL, GREATER, LESS,
    NUM, ID, ERROR
} TokenType;

const std::map<TokenType, ArithmeticOperatorType> arithOperatorType = {
  {PLUS, OPERATOR_PLUS},
  {MINUS, OPERATOR_MINUS},
  {MULT, OPERATOR_MULT},
  {DIV, OPERATOR_DIV},
};


const std::map<TokenType, ConditionalOperatorType> condOperatorType = {
  {GREATER, CONDITION_GREATER},
  {LESS, CONDITION_LESS},
  {NOTEQUAL, CONDITION_NOTEQUAL},
};










class Token {
  public:
    void Print();

    std::string lexeme;
    TokenType token_type;
    int line_no;
};

class LexicalAnalyzer {
  public:
    Token GetToken();
    Token peek(int);
    LexicalAnalyzer();

  private:
    std::vector<Token> tokenList;
    Token GetTokenMain();
    int line_no;
    int index;
    Token tmp;
    InputBuffer input;

    bool SkipSpace();
    int FindKeywordIndex(std::string);
    Token ScanIdOrKeyword();
    Token ScanNumber();
};

#endif  //__LEXER__H__
