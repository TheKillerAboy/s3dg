#include <s3dg/funcintr/ast.h>
#include <s3dg/funcintr/execute.h>
#include <iostream>
#include <cassert>
#include <cmath>
#include <spdlog/spdlog.h>
#include <s3dg/utils/math.h>

namespace s3dg {
    namespace execute {

        Result Result::clone() const {
            return Result(ret);
        }

        ResultPtr Result::clone_ptr() const {
            return init(ret);
        }

        ResultPtr Result::init() {
            return std::make_unique<Result>();
        }

        ResultPtr Result::init(float ret) {
            return std::make_unique<Result>(ret);
        }

        ResultPtr Result::plus(ResultPtr left, ResultPtr right) {
            return Result::init(left->ret + right->ret);
        }
        ResultPtr Result::minus(ResultPtr left, ResultPtr right) {
            return Result::init(left->ret - right->ret);
        }
        ResultPtr Result::mul(ResultPtr left, ResultPtr right) {
            return Result::init(left->ret * right->ret);
        }
        ResultPtr Result::div(ResultPtr left, ResultPtr right) {
            return Result::init(left->ret / right->ret);
        }
        ResultPtr Result::pow(ResultPtr left, ResultPtr right) {
            return Result::init(std::pow(left->ret, right->ret));
        }

        void ExecuteState::set_variable(const std::string& name, ResultPtr val) {
            VariableLookup[name] = std::move(val);
        }
        void ExecuteState::set_function(const std::string& name, std::unique_ptr<ast::ASTExprFuncDefine> expr) {
            FunctionLookup[name] = std::move(expr);
        }

        ResultPtr ExecuteState::get_variable(const std::string& name) {
            return VariableLookup[name]->clone_ptr();
        }

        void LangFunctions::exprs_to_values(execute::ExecuteStatePtr state, std::vector<ast::ASTExprPtr>& exprs, std::vector<ResultPtr>& values, const std::string& context = "") {
            values.reserve(exprs.size());

            for (auto it = exprs.begin(); it != exprs.end(); ++it) {
                auto expr = std::move(*it);
                // std::cout<<context<<std::endl; TODO:: Add proper debug logging
                // std::cout<<context<<' ';
                // expr->debug();
                values.push_back(std::move(expr->execute(state)));
                // std::cout<<context<<' ';
                // expr->debug();
                *it = std::move(expr);
            }
        }

        ResultPtr ExecuteState::execute_function(const std::string& name, std::vector<ast::ASTExprPtr>& exprs) {
            ResultPtr rtn;

            if (LangFuncs.has_function(name)) {
                rtn = std::move(LangFuncs.execute_function(name, shared_from_this(), exprs));
            } else {
                std::vector<ResultPtr> values;
                LangFunctions::exprs_to_values(shared_from_this(), exprs, values, "genericfunc");
                auto param_names = FunctionLookup[name]->param_names;
                VariableLookupType buffer;
                load_vars_into_buffer(param_names, buffer);

                for (size_t i = 0; i < param_names.size(); ++i) {
                    set_variable(param_names[i], std::move(values[i]));
                }

                rtn = std::move(FunctionLookup[name]->execute_definition(shared_from_this()));
                unload_vars_from_buffer(buffer);
            }

            return rtn;
        }

        void ExecuteState::load_vars_into_buffer(const std::vector<std::string>& names, VariableLookupType& buffer) {
            for (auto name : names) {
                buffer[name] = std::move(VariableLookup[name]);
                VariableLookup.erase(name);
            }
        }
        void ExecuteState::unload_vars_from_buffer(VariableLookupType& buffer) {
            for (auto it = buffer.begin(); it != buffer.end(); ++it) {
                VariableLookup[it->first] = std::move(it->second);
            }
        }

        LangFunctions::LangFunctions() {
            funcs["print"] = print;
            funcs["eval"] = eval;
        }

        ResultPtr LangFunctions::execute_function(const std::string& name, ExecuteStatePtr state, std::vector<ast::ASTExprPtr>& arg_values) {
            return std::move(funcs[name](std::move(state), arg_values));
        }

        bool LangFunctions::has_function(const std::string& name) const {
            return funcs.find(name) != funcs.end();
        }

        ResultPtr LangFunctions::Iprint(const std::vector<ResultPtr>& values) {
            size_t size = values.size();

            for (size_t i = 0; i < size - 1; ++i) {
                std::cout << values[i]->ret << ", ";
            }

            if (size > 0) {
                std::cout << values[size - 1]->ret;
            }

            std::cout << std::endl;
            return Result::init();
        }

        ResultPtr LangFunctions::print(ExecuteStatePtr state, std::vector<ast::ASTExprPtr>& arg_values) {
            std::vector<ResultPtr> values;
            exprs_to_values(state, arg_values, values, "printfunc");
            return LangFunctions::Iprint(values);
        }

        void LangFunctions::Ieval1D(ExecuteStatePtr state, const std::string& func_name) {
            float xmin = state->get_variable("__xmin")->ret;
            float xmax = state->get_variable("__xmax")->ret;
            float xinc = state->get_variable("__xinc")->ret;
            std::cout << func_name << ": ";

            for (float i = xmin; !cmpf(i, xmax + xinc); i += xinc) {
                std::vector<ast::ASTExprPtr> arg_values;
                arg_values.push_back(
                    std::make_unique<ast::ASTExprNumber>(
                        std::to_string(i)
                    )
                );
                auto out = state->execute_function(func_name, arg_values);
                std::cout << out->ret;

                if (!cmpf(i, xmax)) {
                    std::cout << ", ";
                }
            }

            std::cout << std::endl;
        }

        ResultPtr LangFunctions::eval(ExecuteStatePtr state, std::vector<ast::ASTExprPtr>& arg_values) {
            for (auto it = arg_values.begin(); it != arg_values.end(); ++it) {
                auto expr = std::move(*it);
                auto* var_expr = dynamic_cast<ast::ASTExprVar*>(expr.get());
                std::string func_name = var_expr->get_name();

                switch (state->get_function_param_count(func_name)) {
                    case 1:
                        LangFunctions::Ieval1D(state, func_name);
                        break;
                }
            }

            return Result::init();
        }

        size_t ExecuteState::get_function_param_count(const std::string& func_name) {
            return FunctionLookup[func_name]->param_count();
        }


        void ExecuteState::debug() {
            spdlog::debug("Execute State:");
            spdlog::debug("\tVariables:");

            for (auto it = VariableLookup.begin(); it != VariableLookup.end(); ++it) {
                spdlog::debug("\t\t- {}: {}", it->first, it->second->ret);
            }

            spdlog::debug("\tFunctions:");

            for (auto it = FunctionLookup.begin(); it != FunctionLookup.end(); ++it) {
                // std::cout<<"\t\t- "<<it->first;//<<"(";
                // auto expr = std::move(it->second);
                // const auto& param_names = expr->param_names;
                // size_t size = param_names.size();
                // for(size_t i = 0; i < size-1; ++i){
                //     std::cout<<param_names[i]<<',';
                // }
                // if(size > 0){
                //     std::cout<<param_names[size-1];
                // }
                // it->second = std::move(expr);
                // std::cout<<")\n";
                // TODO: make work without sigfault
                // std::cout<<'\n';
                spdlog::debug("\t\t- {}", it->first);
            }
        }

        ExecuteState::ExecuteState() {
            set_variable("__xmin", Result::init(-1));
            set_variable("__xmax", Result::init(1));
            set_variable("__xinc", Result::init(0.1));
        }

    }
}
