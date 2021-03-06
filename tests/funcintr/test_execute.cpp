#include <catch2/catch_all.hpp>
#include <s3dg/funcintr/execute.h>
#include <s3dg/funcintr/lexer.h>
#include <s3dg/funcintr/parser.h>
#include <sstream>
#include <iostream>

using namespace s3dg::execute;

TEST_CASE("execute_function print", "[execute]") {
    // redirect cout
    std::streambuf* oldCoutStreamBuf = std::cout.rdbuf();
    std::ostringstream strCout;
    std::cout.rdbuf(strCout.rdbuf());
    SECTION("print") {
        std::stringstream ss(std::string("define f() = 5\nprint f()"));
        auto lexer_queue = s3dg::lexer::lexer(ss);
        auto ast = s3dg::parser::parse_top_level(lexer_queue);
        s3dg::lexer::debug_lexer_queue(lexer_queue);
        auto state = std::make_shared<ExecuteState>();
        ast->execute(state);
        auto out = strCout.str();
        strCout.flush();
        REQUIRE(out == "5\n");
    }
    SECTION("print x") {
        std::stringstream ss(std::string("define f(x) = x\nprint f(2) f(3)"));
        auto lexer_queue = s3dg::lexer::lexer(ss);
        auto ast = s3dg::parser::parse_top_level(lexer_queue);
        auto state = std::make_shared<ExecuteState>();
        ast->execute(state);
        auto out = strCout.str();
        strCout.flush();
        REQUIRE(out == "2, 3\n");
    }
    SECTION("print constant") {
        std::stringstream ss(std::string("define scale = 2\ndefine const = 3\n print scale const"));
        auto lexer_queue = s3dg::lexer::lexer(ss);
        auto ast = s3dg::parser::parse_top_level(lexer_queue);
        auto state = std::make_shared<ExecuteState>();
        ast->execute(state);
        auto out = strCout.str();
        strCout.flush();
        REQUIRE(out == "2, 3\n");
    }
    SECTION("print mul") {
        std::stringstream ss(std::string("define f(x) = x*2\nprint f(2) f(3)"));
        auto lexer_queue = s3dg::lexer::lexer(ss);
        auto ast = s3dg::parser::parse_top_level(lexer_queue);
        auto state = std::make_shared<ExecuteState>();
        ast->execute(state);
        auto out = strCout.str();
        strCout.flush();
        REQUIRE(out == "4, 6\n");
    }
    SECTION("eval simple") {
        std::stringstream ss(std::string("define f(x) = x\neval f"));
        auto lexer_queue = s3dg::lexer::lexer(ss);
        auto ast = s3dg::parser::parse_top_level(lexer_queue);
        auto state = std::make_shared<ExecuteState>();
        ast->execute(state);
        auto out = strCout.str();
        strCout.flush();
        REQUIRE(out == "f: -1, -0.9, -0.8, -0.7, -0.6, -0.5, -0.4, -0.3, -0.2, -0.1, 0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1\n");
    }
    // restore cout
    std::cout.rdbuf(oldCoutStreamBuf);
}
