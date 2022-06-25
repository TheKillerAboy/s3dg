#pragma once
#include <memory>
#include <string>
#include <queue>
#include <cmath>
#include <s3dg/funcintr/execute.h>

namespace s3dg{
namespace ast{

class ASTExpr{
public:
    virtual execute::ResultPtr execute(execute::ExecuteStatePtr state){};
};

typedef std::unique_ptr<ASTExpr> ASTExprPtr;

class ASTExprTopLevel: public ASTExpr{
    std::queue<ASTExprPtr> exprs;
public:
    ASTExprTopLevel(){}
    void add_expr(ASTExprPtr expr);
    execute::ResultPtr execute(execute::ExecuteStatePtr state);
};

class ASTExprConstDefine: public ASTExpr{
    std::string name;
    ASTExprPtr value;

public:
    ASTExprConstDefine(std::string name, ASTExprPtr value): name(name), value(std::move(value)) {}
    std::string get_name() const;
    execute::ResultPtr execute(execute::ExecuteStatePtr state);
};

class ASTExprFuncDefine: public ASTExpr{
    std::string name;
    ASTExprPtr definition;

public:
    std::vector<std::string> param_names;

    ASTExprFuncDefine(std::string name, std::vector<std::string> param_names, ASTExprPtr definition): name(name), param_names(std::move(param_names)), definition(std::move(definition)) {}
    size_t param_count() const;
    std::string get_name() const;

    execute::ResultPtr execute_definition(execute::ExecuteStatePtr state);
};

class ASTExprFuncCall: public ASTExpr{
    std::string name;
    std::vector<ASTExprPtr> arg_values;
public:
    ASTExprFuncCall(std::string name, std::vector<ASTExprPtr> arg_values): name(name), arg_values(std::move(arg_values)){}
    size_t arg_count() const;
    std::string get_name() const;
};



class ASTExprVar: public ASTExpr{
    std::string name;
public:
    ASTExprVar(std::string name): name(name){}
    std::string get_name() const;
};

class ASTExprNumber: public ASTExpr{
    std::string value;
public:
    ASTExprNumber(std::string value): value(value){}
    std::string get_value() const;
};

class ASTExprBinOp: public ASTExpr{
    ASTExprPtr LHS, RHS;
    std::string op;
public:
    ASTExprBinOp(std::string op, ASTExprPtr LHS, ASTExprPtr RHS): op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
};

}
}
