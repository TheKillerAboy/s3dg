#include <s3dg/funcintr/binop.h>
#include <s3dg/funcintr/execute.h>
#include <s3dg/funcintr/ast.h>

namespace s3dg {
namespace binops {

BinOpsSingleton::BinOpsSingleton() {
    metas.insert({
        "+",
        std::make_shared<BinOpMeta>(20,"0")
    });
    metas.insert({
        "-",
        std::make_shared<BinOpMeta>(20,"0")
    });
    metas.insert({
        "*",
        std::make_shared<BinOpMeta>(50,"1")
    });
    metas.insert({
        "/",
        std::make_shared<BinOpMeta>(50,"0")
    });
    metas.insert({
        "^",
        std::make_shared<BinOpMeta>(70,"1")
    });

    result_ptr_funcs.insert({
        "+",
        execute::Result::plus
    });
    result_ptr_funcs.insert({
        "-",
        execute::Result::minus
    });
    result_ptr_funcs.insert({
        "*",
        execute::Result::mul
    });
    result_ptr_funcs.insert({
        "/",
        execute::Result::div
    });
    result_ptr_funcs.insert({
        "^",
        execute::Result::pow
    });
}

BinOpMetaPtr BinOpsSingleton::Iget_meta(const std::string& binop) {
    return metas.find(binop)->second;
}

bool BinOpsSingleton::Ihas_meta(const std::string& binop) {
    return metas.find(binop) != metas.end();
}

BinOpsSingleton& BinOpsSingleton::get() {
    static BinOpsSingleton instance;
    return instance;
}

bool BinOpsSingleton::has_meta(const std::string& binop) {
    return BinOpsSingleton::get().Ihas_meta(binop);
}

BinOpMetaPtr BinOpsSingleton::get_meta(const std::string& binop) {
    return BinOpsSingleton::get().Iget_meta(binop);
}
unsigned int BinOpsSingleton::get_precedence(const std::string& binop) {
    return BinOpsSingleton::get_meta(binop)->precedence;
}
std::string BinOpsSingleton::get_unary_pre(const std::string& binop) {
    return BinOpsSingleton::get_meta(binop)->unary_pre;
}

execute::ResultPtr BinOpsSingleton::Iexecute_result_ptr_func(const std::string& op, execute::ResultPtr left, execute::ResultPtr right) {
    return result_ptr_funcs[op](std::move(left), std::move(right));
}

execute::ResultPtr BinOpsSingleton::execute_result_ptr_func(const std::string& op, execute::ResultPtr left, execute::ResultPtr right) {
    return BinOpsSingleton::get().Iexecute_result_ptr_func(op, std::move(left), std::move(right));
}

}
}
