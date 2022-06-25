#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <s3dg/funcintr/parser.h>
#include <s3dg/funcintr/lexer.h>
#include <sstream>

using namespace s3dg::parser;
TEST_CASE( "parse_top_level_call", "[parser]" ) {
    SECTION("no args"){
        std::stringstream ss(std::string(""));
        auto lexer_queue = s3dg::lexer::lexer(ss);
        auto ast = parse_top_level_call("call", lexer_queue);
        s3dg::ast::ASTExprFuncCall* ast_fc = static_cast<s3dg::ast::ASTExprFuncCall*>(ast.get());
        REQUIRE(ast_fc->arg_count() == 0);
        REQUIRE(ast_fc->get_name() == "call");
        REQUIRE(lexer_queue->empty());
    }
    SECTION("multi args"){
        std::stringstream ss(std::string("scaler 1"));
        auto lexer_queue = s3dg::lexer::lexer(ss);
        // s3dg::lexer::debug_lexer_queue(lexer_queue);
        auto ast = parse_top_level_call("call", lexer_queue);
        s3dg::ast::ASTExprFuncCall* ast_fc = static_cast<s3dg::ast::ASTExprFuncCall*>(ast.get());
        REQUIRE(ast_fc->arg_count() == 2);
        REQUIRE(ast_fc->get_name() == "call");
        REQUIRE(lexer_queue->empty());
    }
}

TEST_CASE( "parse_primary", "[parser]" ) {
    SECTION("number"){
        std::stringstream ss(std::string("1"));
        auto lexer_queue = s3dg::lexer::lexer(ss);
        auto ast = parse_primary(lexer_queue);
        s3dg::ast::ASTExprNumber* ast_fc = static_cast<s3dg::ast::ASTExprNumber*>(ast.get());
        REQUIRE(ast_fc->get_value() == "1");
    }
    SECTION("variable"){
        std::stringstream ss(std::string("scaler"));
        auto lexer_queue = s3dg::lexer::lexer(ss);
        auto ast = parse_primary(lexer_queue);
        s3dg::ast::ASTExprVar* ast_fc = static_cast<s3dg::ast::ASTExprVar*>(ast.get());
        REQUIRE(ast_fc->get_name() == "scaler");
    }
    SECTION("func call"){
        std::stringstream ss(std::string("f(x,1)"));
        auto lexer_queue = s3dg::lexer::lexer(ss);
        auto ast = parse_primary(lexer_queue);
        s3dg::ast::ASTExprFuncCall* ast_fc = static_cast<s3dg::ast::ASTExprFuncCall*>(ast.get());
        REQUIRE(ast_fc->get_name() == "f");
        REQUIRE(ast_fc->arg_count() == 2);
    }
}

TEST_CASE( "parse_func_call", "[parser]" ) {
    SECTION("empty"){
        std::stringstream ss(std::string("()"));
        auto lexer_queue = s3dg::lexer::lexer(ss);
        // s3dg::lexer::debug_lexer_queue(lexer_queue);
        auto ast = parse_func_call("call",lexer_queue);
        s3dg::ast::ASTExprFuncCall* ast_fc = static_cast<s3dg::ast::ASTExprFuncCall*>(ast.get());
        REQUIRE(ast_fc->get_name() == "call");
        REQUIRE(ast_fc->arg_count() == 0);
    }
    SECTION("single number"){
        std::stringstream ss(std::string("(1)"));
        auto lexer_queue = s3dg::lexer::lexer(ss);
        // s3dg::lexer::debug_lexer_queue(lexer_queue);
        auto ast = parse_func_call("call",lexer_queue);
        s3dg::ast::ASTExprFuncCall* ast_fc = static_cast<s3dg::ast::ASTExprFuncCall*>(ast.get());
        REQUIRE(ast_fc->get_name() == "call");
        REQUIRE(ast_fc->arg_count() == 1);
    }
}

TEST_CASE( "parse_define", "[parser]" ) {
    SECTION("constant"){
        std::stringstream ss(std::string("scaler = 5"));
        auto lexer_queue = s3dg::lexer::lexer(ss);
        // s3dg::lexer::debug_lexer_queue(lexer_queue);
        auto ast = parse_define(lexer_queue);
        s3dg::ast::ASTExprConstDefine* ast_fc = static_cast<s3dg::ast::ASTExprConstDefine*>(ast.get());
        REQUIRE(ast_fc->get_name() == "scaler");
    }
    SECTION("plane"){
        std::stringstream ss(std::string("f(x,y) = 5"));
        auto lexer_queue = s3dg::lexer::lexer(ss);
        // s3dg::lexer::debug_lexer_queue(lexer_queue);
        auto ast = parse_define(lexer_queue);
        s3dg::ast::ASTExprFuncDefine* ast_fc = static_cast<s3dg::ast::ASTExprFuncDefine*>(ast.get());
        REQUIRE(ast_fc->get_name() == "f");
        REQUIRE(ast_fc->param_count() == 2);
    }
}
