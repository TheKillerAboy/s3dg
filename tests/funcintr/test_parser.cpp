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
        auto ast = parse_define(lexer_queue);
        s3dg::ast::ASTExprConstDefine* ast_fc = static_cast<s3dg::ast::ASTExprConstDefine*>(ast.get());
        REQUIRE(ast_fc->get_name() == "scaler");
        REQUIRE(lexer_queue->size() == 0);
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
    SECTION("no params"){
        std::stringstream ss(std::string("f() = 5"));
        auto lexer_queue = s3dg::lexer::lexer(ss);
        // s3dg::lexer::debug_lexer_queue(lexer_queue);
        auto ast = parse_define(lexer_queue);
        s3dg::ast::ASTExprFuncDefine* ast_fc = static_cast<s3dg::ast::ASTExprFuncDefine*>(ast.get());
        REQUIRE(ast_fc->get_name() == "f");
        REQUIRE(ast_fc->param_count() == 0);
    }
}

TEST_CASE( "parse_top_level", "[parser]" ) {
    SECTION("print"){
        std::stringstream ss(std::string("define f() = 5\nprint f()"));
        auto lexer_queue = s3dg::lexer::lexer(ss);
        auto ast = parse_top_level(lexer_queue);

        s3dg::ast::ASTExprTopLevel* ast_fc = static_cast<s3dg::ast::ASTExprTopLevel*>(ast.get());
        
        auto ast_define = ast_fc->pop_expr();
        auto ast_print = ast_fc->pop_expr();

        s3dg::ast::ASTExprFuncDefine* ast_define_fc = static_cast<s3dg::ast::ASTExprFuncDefine*>(ast_define.get());
        REQUIRE(ast_define_fc->get_name() == "f");

        s3dg::ast::ASTExprFuncCall* ast_print_fc = static_cast<s3dg::ast::ASTExprFuncCall*>(ast_print.get());
        REQUIRE(ast_print_fc->get_name() == "print");
        REQUIRE(ast_print_fc->arg_count() == 1);
    }
}

TEST_CASE( "parse_expression", "[parser]" ) {
    SECTION("simple series"){
        std::stringstream ss(std::string("1+2+3"));
        auto lexer_queue = s3dg::lexer::lexer(ss);
        auto ast = parse_expression(lexer_queue);

        auto* ast_binop = static_cast<s3dg::ast::ASTExprBinOp*>(ast.get());
        auto* v_1 = static_cast<s3dg::ast::ASTExprNumber*>(ast_binop->LHS.get());
        REQUIRE(v_1->get_value() == "1");
        auto* v_23 = static_cast<s3dg::ast::ASTExprBinOp*>(ast_binop->RHS.get());
        auto* v_2 = static_cast<s3dg::ast::ASTExprNumber*>(v_23->LHS.get());
        REQUIRE(v_2->get_value() == "2");
        auto* v_3 = static_cast<s3dg::ast::ASTExprNumber*>(v_23->RHS.get());
        REQUIRE(v_3->get_value() == "3");
    }
    SECTION("simple series in brackets"){
        std::stringstream ss(std::string("(1+2+3)"));
        auto lexer_queue = s3dg::lexer::lexer(ss);
        auto ast = parse_expression(lexer_queue);

        auto* ast_binop = static_cast<s3dg::ast::ASTExprBinOp*>(ast.get());
        auto* v_1 = static_cast<s3dg::ast::ASTExprNumber*>(ast_binop->LHS.get());
        REQUIRE(v_1->get_value() == "1");
        auto* v_23 = static_cast<s3dg::ast::ASTExprBinOp*>(ast_binop->RHS.get());
        auto* v_2 = static_cast<s3dg::ast::ASTExprNumber*>(v_23->LHS.get());
        REQUIRE(v_2->get_value() == "2");
        auto* v_3 = static_cast<s3dg::ast::ASTExprNumber*>(v_23->RHS.get());
        REQUIRE(v_3->get_value() == "3");
    }
    SECTION("no bracket mul"){
        std::stringstream ss(std::string("1+2*3"));
        auto lexer_queue = s3dg::lexer::lexer(ss);
        auto ast = parse_expression(lexer_queue);

        auto* ast_binop = static_cast<s3dg::ast::ASTExprBinOp*>(ast.get());
        auto* v_1 = static_cast<s3dg::ast::ASTExprNumber*>(ast_binop->LHS.get());
        REQUIRE(v_1->get_value() == "1");
        auto* v_23 = static_cast<s3dg::ast::ASTExprBinOp*>(ast_binop->RHS.get());
        REQUIRE(v_23->op == "*");
        auto* v_2 = static_cast<s3dg::ast::ASTExprNumber*>(v_23->LHS.get());
        REQUIRE(v_2->get_value() == "2");
        auto* v_3 = static_cast<s3dg::ast::ASTExprNumber*>(v_23->RHS.get());
        REQUIRE(v_3->get_value() == "3");
    }
    SECTION("with bracket mul"){
        std::stringstream ss(std::string("(1+2)*3"));
        auto lexer_queue = s3dg::lexer::lexer(ss);
        auto ast = parse_expression(lexer_queue);

        auto* ast_binop = static_cast<s3dg::ast::ASTExprBinOp*>(ast.get());
        REQUIRE(ast_binop->op == "*");
        auto* v_12 = static_cast<s3dg::ast::ASTExprBinOp*>(ast_binop->LHS.get());
        auto* v_3 = static_cast<s3dg::ast::ASTExprNumber*>(ast_binop->RHS.get());
        REQUIRE(v_3->get_value() == "3");
        auto* v_1 = static_cast<s3dg::ast::ASTExprNumber*>(v_12->LHS.get());
        REQUIRE(v_1->get_value() == "1");
        auto* v_2 = static_cast<s3dg::ast::ASTExprNumber*>(v_12->RHS.get());
        REQUIRE(v_2->get_value() == "2");
    }
    SECTION("bracket with bracketed unary"){
        std::stringstream ss(std::string("(-1)*(2+3)"));
        auto lexer_queue = s3dg::lexer::lexer(ss);
        auto ast = parse_expression(lexer_queue);

        auto* ast_binop = static_cast<s3dg::ast::ASTExprBinOp*>(ast.get());
        REQUIRE(ast_binop->op == "*");

        auto* v_0n1 = static_cast<s3dg::ast::ASTExprBinOp*>(ast_binop->LHS.get());
        REQUIRE(v_0n1->op == "-");

        auto* v_23 = static_cast<s3dg::ast::ASTExprBinOp*>(ast_binop->RHS.get());
        REQUIRE(v_23->op == "+");

        auto* v_0 = static_cast<s3dg::ast::ASTExprNumber*>(v_0n1->LHS.get());
        REQUIRE(v_0->get_value() == "0");
        auto* v_n1 = static_cast<s3dg::ast::ASTExprNumber*>(v_0n1->RHS.get());
        REQUIRE(v_n1->get_value() == "1");

        auto* v_2 = static_cast<s3dg::ast::ASTExprNumber*>(v_23->LHS.get());
        REQUIRE(v_2->get_value() == "2");
        auto* v_3 = static_cast<s3dg::ast::ASTExprNumber*>(v_23->RHS.get());
        REQUIRE(v_3->get_value() == "3");
    }
    SECTION("bracket with unary"){
        std::stringstream ss(std::string("-1*(2+3)")); // should be same as `bracket with bracketed unary`
        auto lexer_queue = s3dg::lexer::lexer(ss);
        auto ast = parse_expression(lexer_queue);

        auto* ast_binop = static_cast<s3dg::ast::ASTExprBinOp*>(ast.get());
        REQUIRE(ast_binop->op == "*");

        auto* v_0n1 = static_cast<s3dg::ast::ASTExprBinOp*>(ast_binop->LHS.get());
        REQUIRE(v_0n1->op == "-");

        auto* v_23 = static_cast<s3dg::ast::ASTExprBinOp*>(ast_binop->RHS.get());
        REQUIRE(v_23->op == "+");

        auto* v_0 = static_cast<s3dg::ast::ASTExprNumber*>(v_0n1->LHS.get());
        REQUIRE(v_0->get_value() == "0");
        auto* v_n1 = static_cast<s3dg::ast::ASTExprNumber*>(v_0n1->RHS.get());
        REQUIRE(v_n1->get_value() == "1");

        auto* v_2 = static_cast<s3dg::ast::ASTExprNumber*>(v_23->LHS.get());
        REQUIRE(v_2->get_value() == "2");
        auto* v_3 = static_cast<s3dg::ast::ASTExprNumber*>(v_23->RHS.get());
        REQUIRE(v_3->get_value() == "3");
    }
}
