#include <s3dg/funcintr/ast.h>
#include <s3dg/funcintr/parser.h>
#include <s3dg/funcintr/lexer.h>
#include <memory>
#include <vector>

namespace s3dg {
namespace parser {

void init_BinOpMetas() {
    BinOpMetas.insert({
        "+",
        std::make_shared<BinOpMeta>(
            20,
            "0"
        )
    });
    BinOpMetas.insert({
        "-",
        std::make_shared<BinOpMeta>(
            20,
            "0"
        )
    });
    BinOpMetas.insert({
        "*",
        std::make_shared<BinOpMeta>(
            50,
            "1"
        )
    });
    BinOpMetas.insert({
        "/",
        std::make_shared<BinOpMeta>(
            50,
            "0"
        )
    });
    BinOpMetas.insert({
        "^",
        std::make_shared<BinOpMeta>(
            70,
            "1"
        )
    });
}

void eat_eos(lexer::LexerQueue lexer_queue) {
    auto tok = std::move(lexer_queue->front());
    lexer_queue->pop_front();

    if(tok->type != lexer::tok_eos) {
        // TODO: error handling
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
    if(tok->buf != ")") {
        lexer_queue->push_front(std::move(tok));
        while(true) {
            arg_values.push_back(std::move(parse_expression(lexer_queue)));
            tok = std::move(lexer_queue->front());
            lexer_queue->pop_front();

            if(tok->buf == ")") {
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
    auto tok = std::move(lexer_queue->front());
    lexer_queue->pop_front();

    switch (tok->type) {
    case lexer::tok_ident: {
        auto name = tok->buf;
        tok = std::move(lexer_queue->front());
        lexer_queue->pop_front();
        if(tok->type == lexer::tok_any && tok->buf=="(") {
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

std::pair<std::string, std::shared_ptr<BinOpMeta>> try_get_op(lexer::LexerQueue lexer_queue) {
    auto tok = std::move(lexer_queue->front());
    lexer_queue->pop_front();
    if(tok->type == lexer::tok_any) {
        auto meta = BinOpMetas.find(tok->buf);
        if(meta != BinOpMetas.end()) {
            return std::make_pair(meta->first,std::move(meta->second));
        }
    }
    lexer_queue->push_front(std::move(tok));
    return std::make_pair("", nullptr);
}

ast::ASTExprPtr parse_expression(lexer::LexerQueue lexer_queue) {
    ast::ASTExprPtr LHS;
    std::string op;
    auto binopmeta = try_get_op(lexer_queue);
    if(binopmeta.second) {
        op = binopmeta.first;
        LHS = std::make_unique<ast::ASTExprNumber>(binopmeta.second->leading_value);
    }
    else {
        LHS = parse_primary(lexer_queue);
        binopmeta = try_get_op(lexer_queue);
        if(binopmeta.second) {
            op = binopmeta.first;
        }
        else {
            return LHS;
        }
    }
    return std::make_unique<ast::ASTExprBinOp>(
               op,
               std::move(LHS),
               parse_expression(lexer_queue)
           );

}

ast::ASTExprPtr parse_define(lexer::LexerQueue lexer_queue) {
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

        if(tok->buf == ")" && i == 0) {
            break;
        }

        param_names.push_back(tok->buf);

        tok = std::move(lexer_queue->front());
        lexer_queue->pop_front();

        if(tok->buf == ")") {
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
