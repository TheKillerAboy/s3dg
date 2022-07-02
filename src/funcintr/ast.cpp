#include <s3dg/funcintr/ast.h>
#include <s3dg/funcintr/execute.h>
#include <spdlog/spdlog.h>
#include <iostream>

namespace s3dg {
    namespace ast {

        std::string ASTExprTopLevel::debug_name() {
            return "Top Level";
        }
        std::string ASTExprConstDefine::debug_name() {
            return "Const Define";
        }
        std::string ASTExprFuncDefine::debug_name() {
            return "Func Define";
        }
        std::string ASTExprFuncCall::debug_name() {
            return "Func Call";
        }
        std::string ASTExprVar::debug_name() {
            return "Var";
        }
        std::string ASTExprBinOp::debug_name() {
            return "BinOp";
        }
        std::string ASTExprNumber::debug_name() {
            return "Number";
        }

        void ASTExpr::debug() {
            spdlog::debug("AST Type: {}", debug_name());
        }

        void ASTExprFuncDefine::debug() {
            spdlog::debug("AST Type: {}", debug_name());
            spdlog::debug("\tFunc Name: {}", get_name());
            spdlog::debug("\tArg Count {}", param_count());
        }

        void ASTExprTopLevel::add_expr(ASTExprPtr expr) {
            exprs.push(std::move(expr));
        }

        size_t ASTExprFuncCall::arg_count() const {
            return arg_values.size();
        }

        std::string ASTExprFuncCall::get_name() const {
            return name;
        }

        std::string ASTExprNumber::get_value() const {
            return value;
        }

        std::string ASTExprVar::get_name() const {
            return name;
        }

        size_t ASTExprFuncDefine::param_count() const {
            return param_names.size();
        }
        std::string ASTExprFuncDefine::get_name() const {
            return name;
        }
        std::string ASTExprConstDefine::get_name() const {
            return name;
        }

        ASTExprPtr ASTExprTopLevel::pop_expr() {
            auto rtn = std::move(exprs.front());
            exprs.pop();
            return rtn;
        }

        execute::ResultPtr ASTExprTopLevel::execute(execute::ExecuteStatePtr state) {
            while (!exprs.empty()) {
                auto ast = std::move(exprs.front());
                exprs.pop();
                ast->execute(state);
            }

            return execute::Result::empty_result();
        }

        execute::ResultPtr ASTExprConstDefine::execute(execute::ExecuteStatePtr state) {
            state->set_variable(name, value->execute(state));
            return execute::Result::empty_result();
        }

        execute::ResultPtr ASTExprFuncDefine::execute_definition(execute::ExecuteStatePtr state) {
            return definition->execute(state);
        }

        std::unique_ptr<ASTExprFuncDefine> ASTExprFuncDefine::move() {
            return std::make_unique<ASTExprFuncDefine>(
                       name,
                       std::move(param_names),
                       std::move(definition)
                   );
        }

        execute::ResultPtr ASTExprFuncDefine::execute(execute::ExecuteStatePtr state) {
            auto expr = move();
            state->set_function(name, std::move(expr));
            return execute::Result::empty_result();
        }

        execute::ResultPtr ASTExprFuncCall::execute(execute::ExecuteStatePtr state) {
            return state->execute_function(name, arg_values);
        }

        execute::ResultPtr ASTExprNumber::execute(execute::ExecuteStatePtr state) {
            return std::make_unique<execute::Result>(std::stof(value));
        }

        execute::ResultPtr ASTExprVar::execute(execute::ExecuteStatePtr state) {
            return std::make_unique<execute::Result>(state->get_variable(name)->clone());
        }


    }
}
