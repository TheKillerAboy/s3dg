#pragma once
#include <map>
#include <memory>
#include <functional>
#include <vector>
#include <cmath>

namespace s3dg {

    namespace ast {
        class ASTExpr;
        class ASTExprFuncDefine;
        typedef std::unique_ptr<ASTExpr> ASTExprPtr;
    }

    namespace execute {

        class Result;
        typedef std::unique_ptr<Result> ResultPtr;

        class Result {
          public:
            float ret;
            Result() : ret(NAN) {}
            Result(float ret) : ret(ret) {}
            Result clone() const;
            ResultPtr clone_ptr() const;

            static ResultPtr init();
            static ResultPtr init(float ret);
            static ResultPtr init(const Result& ret);

            static ResultPtr plus(ResultPtr left, ResultPtr right);
            static ResultPtr minus(ResultPtr left, ResultPtr right);
            static ResultPtr mul(ResultPtr left, ResultPtr right);
            static ResultPtr div(ResultPtr left, ResultPtr right);
            static ResultPtr pow(ResultPtr left, ResultPtr right);
        };

        class ExecuteState;

        typedef std::shared_ptr<ExecuteState> ExecuteStatePtr;
        typedef std::map<std::string, ResultPtr> VariableLookupType;
        typedef std::function<ResultPtr(ExecuteStatePtr, std::vector<ast::ASTExprPtr> &)> LangDefinedFunc;

        class LangFunctions {
            std::map<std::string, LangDefinedFunc> funcs;

          private:
            static ResultPtr Iprint(const std::vector<ResultPtr>& values);
            static void Ieval1D(ExecuteStatePtr state, const std::string&);

            static ResultPtr print(ExecuteStatePtr state, std::vector<ast::ASTExprPtr>& arg_values);
            static ResultPtr eval(ExecuteStatePtr state, std::vector<ast::ASTExprPtr>& arg_values);

          public:
            LangFunctions();

            ResultPtr execute_function(const std::string& name, ExecuteStatePtr state, std::vector<ast::ASTExprPtr>& arg_values);
            bool has_function(const std::string& name) const;

            static void exprs_to_values(execute::ExecuteStatePtr state, std::vector<ast::ASTExprPtr>& exprs, std::vector<ResultPtr>& values, const std::string& context);
        };

        class ExecuteState : public std::enable_shared_from_this<ExecuteState> {
            VariableLookupType VariableLookup;
            std::map<std::string, std::unique_ptr<ast::ASTExprFuncDefine>> FunctionLookup;
            LangFunctions LangFuncs;

          private:
            void load_vars_into_buffer(const std::vector<std::string>& names, VariableLookupType& buffer);
            void unload_vars_from_buffer(VariableLookupType& buffer);

          public:
            ExecuteState();

            void set_variable(const std::string& name, ResultPtr val);
            void set_function(const std::string& name, std::unique_ptr<ast::ASTExprFuncDefine> expr);

            ResultPtr get_variable(const std::string& name);
            ResultPtr execute_function(const std::string& name, std::vector<ast::ASTExprPtr>& exprs);
            size_t get_function_param_count(const std::string&);

            void debug();
        };

    }
}
