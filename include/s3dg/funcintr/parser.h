#pragma once
#include <s3dg/funcintr/lexer.h>
#include <s3dg/funcintr/ast.h>
#include <queue>
#include <map>

namespace s3dg{
namespace parser{

class BinOpMeta{
public:
    unsigned int Precedence;
    std::string leading_value;

    BinOpMeta(unsigned int Precedence, std::string leading_value): Precedence(Precedence), leading_value(leading_value){}
};

static std::map<std::string, std::shared_ptr<BinOpMeta>> BinOpMetas;

void init_BinOpMetas();

void eat_eos(lexer::LexerQueue lexer_queue);

std::pair<std::string, std::shared_ptr<BinOpMeta>>  try_get_op(lexer::LexerQueue lexer_queue);

std::unique_ptr<ast::ASTExpr> parse_primary(lexer::LexerQueue lexer_queue);
std::unique_ptr<ast::ASTExpr> parse_expression(lexer::LexerQueue lexer_queue);

std::unique_ptr<ast::ASTExpr> parse_define(lexer::LexerQueue lexer_queue);

std::unique_ptr<ast::ASTExpr> parse_func_call(
    std::string name,
    lexer::LexerQueue lexer_queue
);

std::unique_ptr<ast::ASTExpr> parse_top_level_call(
    std::string name,
    lexer::LexerQueue lexer_queue
);

std::unique_ptr<ast::ASTExprTopLevel> parse_top_level(lexer::LexerQueue lexer_queue);

}
}
