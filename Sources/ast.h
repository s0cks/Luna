#ifndef LUNA_AST_H
#define LUNA_AST_H

#include "scope.h"
#include "type.h"
#include "array.h"
#include "parser.h"

namespace Luna{
    class Instruction;

#define AST_NODES(V) \
    V(Return) \
    V(Literal) \
    V(CompoundCall) \
    V(Local) \
    V(StoreLocal) \
    V(LoadLocal) \
    V(BinaryOp)

#define FORWARD_DECLARE(Name) class Name##Node;
    AST_NODES(FORWARD_DECLARE)
#undef FORWARD_DECLARE

#define AST_COMMON_FUNCTIONS \
    virtual const char* Name(); \
    virtual void EmitInstructions(std::vector<Instruction*>* instrs);

    class Token;

    class AstNode{
    public:
        AstNode(){}
        virtual ~AstNode(){}

        virtual AstNode* MakeAssignmentNode(AstNode* rhs){ return nullptr; }
        virtual Object* EvalConstantExpr(){ return nullptr; }
        virtual bool IsConstant(){ return false; }
        virtual const char* Name(){ return ""; }
        virtual void EmitInstructions(std::vector<Instruction*>* instrs){ return; }

#define AST_TYPE_CHECK(BaseName) \
        bool Is##BaseName##Node(){ return As##BaseName##Node() != nullptr; } \
        virtual BaseName##Node* As##BaseName##Node(){ return nullptr; }
    AST_NODES(AST_TYPE_CHECK)
#undef AST_TYPE_CHECK
    };

    class LocalNode: public AstNode{
    public:
        LocalNode(): vars_(1), inits_(1), nodes_(1){}

        LocalVariable* AddInitializer(Token* token, AstNode* node);
        virtual bool IsConstant();
        virtual Object* EvalConstantExpr();

        AST_COMMON_FUNCTIONS;
    private:
        Array<LocalVariable*> vars_;
        Array<AstNode*> inits_;
        Array<AstNode*> nodes_;
    };

    class StoreLocalNode: public AstNode{
    public:
        StoreLocalNode(LocalVariable* local, AstNode* value):
                local_(*local),
                value_(value){}

        LocalVariable Local(){
            return local_;
        }

        AstNode* Value(){
            return value_;
        }

        AST_COMMON_FUNCTIONS;
    private:
        LocalVariable local_;
        AstNode* value_;
    };

    class LiteralNode: public AstNode{
    public:
        LiteralNode(Object* literal):
                literal_(literal){}

        Object* Literal(){
            return literal_;
        }

        virtual bool IsConstant(){
            return true;
        }

        virtual LiteralNode* AsLiteralNode(){
            return this;
        }

        virtual Object* EvalConstantExpr(){
            return literal_;
        }

        AST_COMMON_FUNCTIONS;
    private:
        Object* literal_;
    };

    class ReturnNode: public AstNode{
    public:
        ReturnNode():
                value_(new LiteralNode(Object::NIL)){}

        ReturnNode(AstNode* value):
                value_(value){}

        AstNode* Value(){
            return value_;
        }

        void SetScope(Scope* scope){
            scope_ = scope;
        }

        Scope* GetScope(){
            return scope_;
        }

        AST_COMMON_FUNCTIONS;
    private:
        AstNode* value_;
        Scope* scope_;
    };

    class CompoundCallNode: public AstNode{
    public:
        CompoundCallNode(AstNode* recv, const std::string& name /*, ArgumentListNode args*/):
                receiver_(recv),
                name_(name) /*, arguments_(args) */{}

        AstNode* Receiver(){
            return receiver_;
        }

        std::string FunctionName(){
            return name_;
        }

        AST_COMMON_FUNCTIONS;
    private:
        AstNode* receiver_;
        std::string name_;
    };

    class LoadLocalNode: public AstNode{
    public:
        LoadLocalNode(LocalVariable* local):
                local_(*local){}

        LocalVariable Local(){
            return local_;
        }

        virtual Object* EvalConstantExpr();
        virtual bool IsConstant();
        virtual AstNode* MakeAssignmentNode(AstNode* rhs);

        AST_COMMON_FUNCTIONS;
    private:
        LocalVariable local_;
    };

    class BinaryOpNode: public AstNode{
    public:
        BinaryOpNode(TokenKind op, AstNode* left, AstNode* right):
                op_(op),
                left_(left),
                right_(right){}

        TokenKind Op(){
            return op_;
        }


        AstNode* LHS(){
            return left_;
        }

        AstNode* RHS(){
            return right_;
        }

        virtual bool IsConstant();
        virtual Object* EvalConstantExpr();

        AST_COMMON_FUNCTIONS;
    private:
        AstNode* left_;
        AstNode* right_;
        TokenKind op_;
    };
}

#endif