#include <s3dg/funcintr/ast.h>
#include <s3dg/funcintr/execute.h>
#include <iostream>
#include <cassert>
#include <cmath>
#include <spdlog/spdlog.h>

namespace s3dg {
namespace execute {

Result Result::clone() const {
    return Result(ret);
}

ResultPtr Result::empty_result() {
    return std::make_unique<Result>();
}

ResultPtr Result::plus(ResultPtr left, ResultPtr right){
    return std::make_unique<Result>(left->ret + right->ret);
}
ResultPtr Result::minus(ResultPtr left, ResultPtr right){
    return std::make_unique<Result>(left->ret - right->ret);
}
ResultPtr Result::mul(ResultPtr left, ResultPtr right){
    return std::make_unique<Result>(left->ret*right->ret);
}
ResultPtr Result::div(ResultPtr left, ResultPtr right){
    return std::make_unique<Result>(left->ret / right->ret);
}
ResultPtr Result::pow(ResultPtr left, ResultPtr right){
    return std::make_unique<Result>(std::pow(left->ret, right->ret));
}

void ExecuteState::set_variable(const std::string& name, ResultPtr val) {
    VariableLookup[name] = std::move(val);
}
void ExecuteState::set_function(const std::string& name, std::unique_ptr<ast::ASTExprFuncDefine> expr) {
    FunctionLookup[name] = std::move(expr);
}

ResultPtr ExecuteState::get_variable(const std::string& name) {
    return std::make_unique<Result>(VariableLookup[name]->clone());
}

void LangFunctions::exprs_to_values(execute::ExecuteStatePtr state, std::vector<ast::ASTExprPtr>& exprs, std::vector<ResultPtr>& values, const std::string& context = "") {
    values.reserve(exprs.size());
    for(auto it = exprs.begin(); it != exprs.end(); ++it) {
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
    if(LangFuncs.has_function(name)) {
        rtn = std::move(LangFuncs.execute_function(name, shared_from_this(), exprs));
    }
    else {
        std::vector<ResultPtr> values;
        LangFunctions::exprs_to_values(shared_from_this(),exprs, values, "genericfunc");

        auto param_names = FunctionLookup[name]->param_names;

        VariableLookupType buffer;
        load_vars_into_buffer(param_names, buffer);
        for(size_t i = 0; i < param_names.size(); ++i) {
            set_variable(param_names[i], std::move(values[i]));
        }
        rtn = std::move(FunctionLookup[name]->execute_definition(shared_from_this()));
        unload_vars_from_buffer(buffer);
    }
    return rtn;
}

void ExecuteState::load_vars_into_buffer(const std::vector<std::string>& names, VariableLookupType& buffer) {
    for(auto name: names) {
        buffer[name] = std::move(VariableLookup[name]);
        VariableLookup.erase(name);
    }
}
void ExecuteState::unload_vars_from_buffer(VariableLookupType& buffer) {
    for(auto it = buffer.begin(); it != buffer.end(); ++it) {
        VariableLookup[it->first] = std::move(it->second);
    }
}

LangFunctions::LangFunctions() {
    funcs["print"] = print;
}

ResultPtr LangFunctions::execute_function(const std::string& name, ExecuteStatePtr state, std::vector<ast::ASTExprPtr>& arg_values) {
    return std::move(funcs[name](std::move(state), arg_values));
}

bool LangFunctions::has_function(const std::string& name) const {
    return funcs.find(name) != funcs.end();
}

ResultPtr LangFunctions::print(ExecuteStatePtr state, std::vector<ast::ASTExprPtr>& arg_values) {
    std::vector<ResultPtr> values;
    exprs_to_values(state, arg_values, values, "printfunc");

    size_t size = values.size();
    for(size_t i = 0; i < size-1; ++i) {
        std::cout<<values[i]->ret<<", ";
    }
    if(size>0) {
        std::cout<<values[size-1]->ret;
    }
    std::cout<<std::endl;

    return Result::empty_result();
}

void ExecuteState::debug() {
    spdlog::debug("Execute State:");
    spdlog::debug("\tVariables:");
    for(auto it = VariableLookup.begin(); it != VariableLookup.end(); ++it) {
        spdlog::debug("\t\t- {}: {}", it->first, it->second->ret);
    }
    spdlog::debug("\tFunctions:");
    for(auto it = FunctionLookup.begin(); it != FunctionLookup.end(); ++it) {
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

}
}
