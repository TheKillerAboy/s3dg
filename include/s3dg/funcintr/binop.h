#pragma once
#include <string>
#include <map>
#include <memory>

namespace s3dg {
    namespace binops {

        class BinOpMeta {
          public:
            unsigned int precedence;
            std::string unary_pre;
            BinOpMeta(unsigned int p, const std::string& u): precedence(p), unary_pre(u) {}
        };

        typedef std::shared_ptr<BinOpMeta> BinOpMetaPtr;

        class BinOpsSingleton {
            std::map<std::string, BinOpMetaPtr> metas;
          private:
            BinOpsSingleton();
            BinOpsSingleton(const BinOpsSingleton&) = delete;

            BinOpMetaPtr Iget_meta(const std::string&);
            bool Ihas_meta(const std::string&);
          public:
            static BinOpsSingleton& get();
            static bool has_meta(const std::string&);
            static BinOpMetaPtr get_meta(const std::string&);
            static unsigned int get_precedence(const std::string&);
            static std::string get_unary_pre(const std::string&);
        };

    }
}
