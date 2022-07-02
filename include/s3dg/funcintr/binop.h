#pragma once
#include <string>
#include <map>
#include <memory>
#include <functional>
#include <s3dg/funcintr/execute.h>

namespace s3dg {
    namespace binops {

        class BinOpMeta {
          public:
            unsigned int precedence;
            std::string unary_pre;
            BinOpMeta(unsigned int p, const std::string& u): precedence(p), unary_pre(u) {}
        };

        typedef std::shared_ptr<BinOpMeta> BinOpMetaPtr;
        typedef std::function<execute::ResultPtr(execute::ResultPtr, execute::ResultPtr)> BinOpResultPtrFunc;

        class BinOpsSingleton {
            std::map<std::string, BinOpMetaPtr> metas;
            std::map<std::string, BinOpResultPtrFunc> result_ptr_funcs;
          private:
            BinOpsSingleton();
            BinOpsSingleton(const BinOpsSingleton&) = delete;

            BinOpMetaPtr Iget_meta(const std::string&);
            bool Ihas_meta(const std::string&);
            execute::ResultPtr Iexecute_result_ptr_func(const std::string&, execute::ResultPtr, execute::ResultPtr);
          public:
            static BinOpsSingleton& get();
            static bool has_meta(const std::string&);
            static BinOpMetaPtr get_meta(const std::string&);
            static unsigned int get_precedence(const std::string&);
            static std::string get_unary_pre(const std::string&);
            static execute::ResultPtr execute_result_ptr_func(const std::string&, execute::ResultPtr, execute::ResultPtr);
        };

    }
}
