#include <catch2/catch_all.hpp>
#include <s3dg/funcintr/lexer.h>
#include <sstream>
#include <string>

using namespace s3dg::lexer;

TEST_CASE( "get_nonblank", "[lexer]" ) {
    SECTION("leading whitespace"){
        std::stringstream ss(std::string(" b"));

        REQUIRE(get_nonblank(ss) == 'b');
    }
    SECTION("empty"){
        std::stringstream ss(std::string(""));
        char c = get_nonblank(ss);
        REQUIRE((c == 0 || c == EOF));
    }
    SECTION("no leading whitespace"){
        std::stringstream ss(std::string("d"));
        char c;

        REQUIRE(get_nonblank(ss) == 'd');
    }
}

TEST_CASE("get_tok", "[lexer]"){
    SECTION("eos"){
        std::stringstream ss(std::string(""));
        auto tok = get_tok(ss);
        REQUIRE(tok->type == TokenType::tok_eos);
    }
    SECTION("eos after"){
        std::stringstream ss(std::string("define"));
        auto tok = get_tok(ss);
        tok = get_tok(ss);
        REQUIRE(tok->type == TokenType::tok_eos);
    }
    SECTION("eos newline"){
        std::stringstream ss(std::string("\n"));
        auto tok = get_tok(ss);
        REQUIRE(tok->type == TokenType::tok_eos);
    }
    SECTION("define"){
        std::stringstream ss(std::string("define"));
        auto tok = get_tok(ss);
        REQUIRE(tok->type == TokenType::tok_define);
    }
    SECTION("number"){
        std::stringstream ss(std::string("12.333fine"));
        auto tok = get_tok(ss);
        REQUIRE(tok->type == TokenType::tok_number);
        REQUIRE(tok->buf == "12.333");
    }
    SECTION("any"){
        std::stringstream ss(std::string("+-)("));
        auto tok = get_tok(ss);
        REQUIRE(tok->type == TokenType::tok_any);
        REQUIRE(tok->buf == "+");
        tok = get_tok(ss);
        REQUIRE(tok->type == TokenType::tok_any);
        REQUIRE(tok->buf == "-");
        tok = get_tok(ss);
        REQUIRE(tok->type == TokenType::tok_any);
        REQUIRE(tok->buf == ")");
        tok = get_tok(ss);
        REQUIRE(tok->type == TokenType::tok_any);
        REQUIRE(tok->buf == "(");
    }
}

TEST_CASE("lexer", "[lexer]"){
    SECTION("std define types"){
        std::stringstream ss(std::string("define scaler = 1"));
        auto lex = lexer(ss);
        REQUIRE(lex->size() == 5);

        REQUIRE(lex->front()->type == TokenType::tok_define);
        lex->pop_front();
        REQUIRE(lex->front()->type == TokenType::tok_ident);
        REQUIRE(lex->front()->buf == "scaler");
        lex->pop_front();
        REQUIRE(lex->front()->type == TokenType::tok_any);
        REQUIRE(lex->front()->buf == "=");
        lex->pop_front();
        REQUIRE(lex->front()->type == TokenType::tok_number);
        REQUIRE(lex->front()->buf == "1");
        lex->pop_front();
        REQUIRE(lex->front()->type == TokenType::tok_eos);
        lex->pop_front();

        REQUIRE(lex->empty());
    }
    SECTION("multi"){
        std::stringstream ss(std::string("call1\ncall2"));
        auto lex = lexer(ss);
        REQUIRE(lex->size() == 4);

        REQUIRE(lex->front()->type == TokenType::tok_ident);
        REQUIRE(lex->front()->buf == "call1");
        lex->pop_front();
        REQUIRE(lex->front()->type == TokenType::tok_eos);
        lex->pop_front();
        REQUIRE(lex->front()->type == TokenType::tok_ident);
        REQUIRE(lex->front()->buf == "call2");
        lex->pop_front();
        REQUIRE(lex->front()->type == TokenType::tok_eos);
        lex->pop_front();

        REQUIRE(lex->empty());
    }
    SECTION("empty"){
        std::stringstream ss(std::string(""));
        auto lex = lexer(ss);
        REQUIRE(lex->size() == 1);

        REQUIRE(lex->front()->type == TokenType::tok_eos);
        lex->pop_front();

        REQUIRE(lex->empty());
    }
}
