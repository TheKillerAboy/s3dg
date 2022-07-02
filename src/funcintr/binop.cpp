#include <s3dg/funcintr/binop.h>

namespace s3dg {
    namespace binops {

        BinOpsSingleton::BinOpsSingleton() {
            metas.insert({
                "+",
                std::make_shared<BinOpMeta>(20, "0")
            });
            metas.insert({
                "-",
                std::make_shared<BinOpMeta>(20, "0")
            });
            metas.insert({
                "*",
                std::make_shared<BinOpMeta>(50, "1")
            });
            metas.insert({
                "/",
                std::make_shared<BinOpMeta>(50, "0")
            });
            metas.insert({
                "^",
                std::make_shared<BinOpMeta>(70, "1")
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

    }
}
