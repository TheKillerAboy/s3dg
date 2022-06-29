#include <s3dg/funcintr/ast.h>
#include <s3dg/funcintr/parser.h>
#include <s3dg/funcintr/lexer.h>
#include <s3dg/funcintr/binop.h>
#include <spdlog/spdlog.h>
#include <memory>
#include <vector>

namespace s3dg {
namespace parser {

void eat_eos(lexer::LexerQueue lexer_queue) {
    // spdlog::debug("Eat EOS");
    // lexer::debug_lexer_queue(lexer_queue);
    if(lexer::is_next_tok(lexer_queue, lexer::tok_eos)) {
        lexer_queue->pop_front();
    }
}

ast::ASTExprPtr parse_func_call(
    std::string name,
    lexer::LexerQueue lexer_queue
) {
    auto tok = std::move(lexer_queue->front());
    lexer_queue->pop_front(); // pop (

    std::vector<ast::ASTExprPtr> arg_values;
    tok = std::move(lexer_queue->front());
    lexer_queue->pop_front(); // pop (
    if(tok->type != lexer::tok_cbrac) {
        lexer_queue->push_front(std::move(tok));
        while(true) {
            arg_values.push_back(std::move(parse_expression(lexer_queue)));
            tok = std::move(lexer_queue->front());
            lexer_queue->pop_front();

            if(tok->type == lexer::tok_cbrac) {
                break;
            }
        }
    }

    return std::make_unique<ast::ASTExprFuncCall>(
               name,
               std::move(arg_values)
           );

}


ast::ASTExprPtr parse_primary(lexer::LexerQueue lexer_queue) {
    //parse single expressions without seps

    // spdlog::debug("Parse Primary:");
    // lexer::debug_lexer_queue(lexer_queue);
    auto tok = std::move(lexer_queue->front());
    lexer_queue->pop_front();

    switch (tok->type) {
    case lexer::tok_ident: {
        auto name = tok->buf;
        tok = std::move(lexer_queue->front());
        lexer_queue->pop_front();
        if(tok->type==lexer::tok_obrac) {
            lexer_queue->push_front(std::move(tok));
            return parse_func_call(
                       name,
                       lexer_queue
                   );
        }
        lexer_queue->push_front(std::move(tok));
        return std::make_unique<ast::ASTExprVar>(name);
    }
    case lexer::tok_number:
        return std::make_unique<ast::ASTExprNumber>(tok->buf);
    }
}

std::pair<std::string, binops::BinOpMetaPtr> try_get_op(lexer::LexerQueue lexer_queue) {
    auto tok = std::move(lexer_queue->front());
    lexer_queue->pop_front();
    if(tok->type == lexer::tok_any) {
        if(binops::BinOpsSingleton::has_meta(tok->buf)) {
            return std::make_pair(tok->buf,std::move(binops::BinOpsSingleton::get_meta(tok->buf)));
        }
    }
    lexer_queue->push_front(std::move(tok));
    return std::make_pair("", nullptr);
}

ast::ASTExprPtr parse_unary(lexer::LexerQueue lexer_queue) {
    spdlog::debug("Parse Unary:");
    lexer::debug_lexer_queue(lexer_queue);
    auto binop = try_get_op(lexer_queue);
    if(binop.second) {
        return std::make_unique<ast::ASTExprBinOp>(
                   binop.first,
                   std::make_unique<ast::ASTExprNumber>(binop.second->unary_pre),
                   std::move(parse_expression(lexer_queue, false))
               );
    }
    return nullptr;
}

ast::ASTExprPtr parse_series(lexer::LexerQueue lexer_queue, ast::ASTExprPtr expr) {
    spdlog::debug("Parse Series:");
    lexer::debug_lexer_queue(lexer_queue);
    // a defined expr is LHS
    if(expr == nullptr)
        expr = std::move(parse_expression(lexer_queue, false));

    if(parse_series_normal_breakpoint(lexer_queue)) {
        auto binop = try_get_op(lexer_queue);
        auto n_expr = std::move(parse_expression(lexer_queue));
        expr = std::make_unique<ast::ASTExprBinOp>(
                   binop.first,
                   std::move(expr),
                   std::move(n_expr)
               );
    }
    return expr;
}

bool parse_series_normal_breakpoint(lexer::LexerQueue lexer_queue) {
    return lexer_queue->front()->type == lexer::tok_any && binops::BinOpsSingleton::has_meta(lexer_queue->front()->buf);
}

ast::ASTExprPtr parse_expression(lexer::LexerQueue lexer_queue, bool can_parse_series) {
    spdlog::debug("Parse Expression ps:{}:", can_parse_series);
    lexer::debug_lexer_queue(lexer_queue);
    auto expr = std::move(parse_unary(lexer_queue));
    if(expr) {
        return parse_series(lexer_queue, std::move(expr));
    }

    if(lexer::is_next_tok(lexer_queue, lexer::tok_obrac)) {
        lexer_queue->pop_front();
        expr = std::move(parse_series(lexer_queue));
        lexer_queue->pop_front();
    }

    if(can_parse_series)
        return parse_series(lexer_queue, std::move(expr));

    return parse_primary(lexer_queue);
}

ast::ASTExprPtr parse_define(lexer::LexerQueue lexer_queue) {
    spdlog::debug("Define:");
    lexer::debug_lexer_queue(lexer_queue);
    auto tok = std::move(lexer_queue->front());
    lexer_queue->pop_front();

    if(tok->type != lexer::tok_ident) {
        // TODO: error handeling
    }

    auto name = tok->buf;

    tok = std::move(lexer_queue->front());
    lexer_queue->pop_front();


    if(tok->type == lexer::tok_any && tok->buf == "=") {
        auto const_define = std::make_unique<ast::ASTExprConstDefine>(
                                name,
                                parse_expression(lexer_queue)
                            );
        eat_eos(lexer_queue);
        return const_define;
    }

    // check (

    std::vector<std::string> param_names;
    int i = 0;
    while(true) {
        tok = std::move(lexer_queue->front());
        lexer_queue->pop_front();

        if(tok->type == lexer::tok_cbrac && i == 0) {
            break;
        }

        param_names.push_back(tok->buf);

        tok = std::move(lexer_queue->front());
        lexer_queue->pop_front();

        if(tok->type == lexer::tok_cbrac) {
            break;
        }
        ++i;
    }

    lexer_queue->pop_front(); // pops =

    auto func_define =  std::make_unique<ast::ASTExprFuncDefine>(
                            name,
                            param_names,
                            parse_expression(lexer_queue)
                        );
    eat_eos(lexer_queue);
    return func_define;
}

ast::ASTExprPtr parse_top_level_call(
    std::string name,
    lexer::LexerQueue lexer_queue
) {
    std::vector<ast::ASTExprPtr> arg_values;
    while(lexer_queue->front()->type != lexer::tok_eos) {
        arg_values.push_back(std::move(parse_expression(lexer_queue)));
        // arg_values.back()->debug();
        // lexer::debug_lexer_queue(lexer_queue);
    }
    eat_eos(lexer_queue);
    return std::make_unique<ast::ASTExprFuncCall>(
               name,
               std::move(arg_values)
           );
}

std::unique_ptr<ast::ASTExprTopLevel> parse_top_level(lexer::LexerQueue lexer_queue) {
    auto top_level_expr = std::make_unique<ast::ASTExprTopLevel>();
    while(!lexer_queue->empty()) {
        auto tok = std::move(lexer_queue->front());
        lexer_queue->pop_front();

        switch(tok->type) {
        case lexer::tok_define:
            top_level_expr->add_expr(
                std::move(
                    parse_define(
                        lexer_queue
                    )
                )
            );
            break;
        case lexer::tok_ident:
            top_level_expr->add_expr(
                std::move(
                    parse_top_level_call(
                        tok->buf,
                        lexer_queue
                    )
                )
            );
            break;
        }
    }
    return top_level_expr;
}

}
}
