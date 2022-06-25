#pragma once
#include <map>
#include <memory>

namespace s3dg{

namespace ast{
    class ASTExprFuncDefine;
}

namespace execute{

class Result{
    float ret;
public:
    Result(): ret(NAN){}
    Result(float ret): ret(ret){}
    Result clone() const;
};

typedef std::unique_ptr<Result> ResultPtr;

class ExecuteState{
    std::map<std::string, ResultPtr> VariableLookup;
    std::map<std::string, ResultPtr> VariableBuffer;
    std::map<std::string, std::unique_ptr<ast::ASTExprFuncDefine>> FunctionLookup;

private:
    void load_vars_into_buffer(const std::vector<std::string>& names);
    void unload_vars_from_buffer(const std::vector<std::string>& names);

public:
    void set_variable(std::string name, ResultPtr val);
    void set_function(std::string name, std::unique_ptr<ast::ASTExprFuncDefine> expr);

    ResultPtr get_variable(std::string name);
    ResultPtr execute_function(std::string name, std::vector<ResultPtr> values);
};

typedef std::shared_ptr<ExecuteState> ExecuteStatePtr;

}
}

