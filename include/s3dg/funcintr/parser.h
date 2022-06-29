#pragma once
#include <s3dg/funcintr/lexer.h>
#include <s3dg/funcintr/ast.h>
#include <s3dg/funcintr/binop.h>
#include <queue>
#include <map>

namespace s3dg{
namespace parser{

void eat_eos(lexer::LexerQueue lexer_queue);

std::pair<std::string, binops::BinOpMetaPtr>  try_get_op(lexer::LexerQueue lexer_queue);

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
