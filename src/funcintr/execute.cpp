#include <s3dg/funcintr/ast.h>
#include <s3dg/funcintr/execute.h>

namespace s3dg {
namespace execute {

Result Result::clone() const {
    return Result(ret);
}

void ExecuteState::set_variable(std::string name, ResultPtr val) {
    VariableLookup[name] = std::move(val);
}
void ExecuteState::set_function(std::string name, std::unique_ptr<ast::ASTExprFuncDefine> expr) {
    FunctionLookup[name] = std::move(expr);
}

ResultPtr ExecuteState::get_variable(std::string name) {
    return std::make_unique<Result>(VariableLookup[name]->clone());
}
ResultPtr ExecuteState::execute_function(std::string name, std::vector<ResultPtr> values);
auto param_names = FunctionLookup[name]->param_names;
load_vars_into_buffer(param_names);
for(size_t i = 0; i < param_names.size(); ++i) {
    set_variable(param_names[i], std::move(values[i]));
}
auto rtn = std::move(FunctionLookup[name]->execute_definition(this));
unload_vars_from_buffer(param_names);
return rtn;
}
}
