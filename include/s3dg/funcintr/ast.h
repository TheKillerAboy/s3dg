#pragma once
#include <memory>
#include <string>
#include <queue>
#include <cmath>
#include <s3dg/funcintr/execute.h>

namespace s3dg {
    namespace ast {

        class ASTExpr {
          private:
            template<class T>
            static bool is_ast_type(ASTExpr*);

          public:
            virtual std::string debug_name() = 0;
            virtual execute::ResultPtr execute(execute::ExecuteStatePtr state) = 0;
            void debug();
        };

        typedef std::unique_ptr<ASTExpr> ASTExprPtr;

        class ASTExprTopLevel: public ASTExpr {
            std::queue<ASTExprPtr> exprs;
          private:
            std::string debug_name();
          public:
            ASTExprTopLevel() {}
            void add_expr(ASTExprPtr expr);
            execute::ResultPtr execute(execute::ExecuteStatePtr state);

            ASTExprPtr pop_expr();
        };

        class ASTExprConstDefine: public ASTExpr {
            std::string name;

          private:
            std::string debug_name();
          public:
            ASTExprPtr value;
            ASTExprConstDefine(const std::string& name, ASTExprPtr value): name(name), value(std::move(value)) {}
            std::string get_name() const;
            execute::ResultPtr execute(execute::ExecuteStatePtr state);
        };

        class ASTExprFuncDefine: public ASTExpr {
            std::string name;
            ASTExprPtr definition;

          private:
            std::string debug_name();
          public:
            std::vector<std::string> param_names;

            ASTExprFuncDefine(const std::string& name, std::vector<std::string> param_names, ASTExprPtr definition): name(name), param_names(std::move(param_names)), definition(std::move(definition)) {}
            size_t param_count() const;
            std::string get_name() const;

            execute::ResultPtr execute_definition(execute::ExecuteStatePtr state);
            execute::ResultPtr execute(execute::ExecuteStatePtr state);

            std::unique_ptr<ASTExprFuncDefine> move();

            void debug();
        };

        class ASTExprFuncCall: public ASTExpr {
            std::string name;
            std::vector<ASTExprPtr> arg_values;
          private:
            std::string debug_name();
          public:
            ASTExprFuncCall(const std::string& name, std::vector<ASTExprPtr> arg_values): name(name), arg_values(std::move(arg_values)) {}
            size_t arg_count() const;
            std::string get_name() const;

            execute::ResultPtr execute(execute::ExecuteStatePtr state);
        };



        class ASTExprVar: public ASTExpr {
            std::string name;
          private:
            std::string debug_name();
          public:
            ASTExprVar(const std::string& name): name(name) {}
            std::string get_name() const;

            execute::ResultPtr execute(execute::ExecuteStatePtr state);
        };

        class ASTExprNumber: public ASTExpr {
            std::string value;
          private:
            std::string debug_name();
          public:
            ASTExprNumber(std::string value): value(value) {}
            std::string get_value() const;

            execute::ResultPtr execute(execute::ExecuteStatePtr state);
        };

        class ASTExprBinOp: public ASTExpr {
          private:
            std::string debug_name();
          public:
            ASTExprPtr LHS, RHS;
            std::string op;
            ASTExprBinOp(const std::string& op, ASTExprPtr LHS, ASTExprPtr RHS): op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

            execute::ResultPtr execute(execute::ExecuteStatePtr state);
        };

    }
}
