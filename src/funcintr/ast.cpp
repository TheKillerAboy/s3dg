#include <s3dg/funcintr/ast.h>
#include <s3dg/funcintr/execute.h>

namespace s3dg {
namespace ast {

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

execute::ResultPtr ASTExprTopLevel::execute(execute::ExecuteStatePtr state) {
    while(!exprs.empty()) {
        auto ast = std::move(exprs.front());
        exprs.pop();

        ast->execute(state);
    }
    return std::make_unique<execute::Result>();
}

execute::ResultPtr ASTExprConstDefine::execute(execute::ExecuteStatePtr state) {
    state->set_variable(name, value->execute(state));
}

}
}
