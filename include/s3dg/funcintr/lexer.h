#pragma once

#include <memory>
#include <sstream>
#include <string>
#include <deque>


namespace s3dg{
namespace lexer{

enum TokenType{
    tok_any = 0,
    tok_ident = 1,
    tok_define = 2,
    tok_number = 4,
    tok_eos = 5 // end of statment
};

struct Token{
    TokenType type;
    std::string buf;
};

char get_nonblank(std::stringstream& ss);

std::unique_ptr<Token> get_tok(std::stringstream& ss);

typedef std::shared_ptr<std::deque<std::unique_ptr<Token>>> LexerQueue;

LexerQueue lexer(std::stringstream& ss);

void debug_lexer_queue(LexerQueue lexer_queue);

}
}
