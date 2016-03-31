#include "ast.h"
#include "assembler.h"
#include "intermediate_language.h"

namespace Luna {
#define DEFINE_NAME_FUNCTION(BaseName) \
    const char* BaseName##Node::Name() { \
        return #BaseName; \
    }

    AST_NODES(DEFINE_NAME_FUNCTION)
#undef DEFINE_NAME_FUNCTION

    LocalVariable* LocalNode::AddInitializer(Token* token, AstNode* node) {
        inits_.Push(node);
        LocalVariable* tmp = new LocalVariable(token->GetText(), kUnknownTID);
        vars_.Push(tmp);
        return tmp;
    }

    bool LocalNode::IsConstant() {
        int i;
        for (i = 0; i < inits_.Length(); i++) {
            if (!inits_.At(i)->IsConstant()) {
                return false;
            }
        }

        for (i = 0; i < nodes_.Length(); i++) {
            if (!nodes_.At(i)->IsConstant()) {
                return false;
            }
        }

        return true;
    }

    Object* LocalNode::EvalConstantExpr() {
        int i = 0;
        for (i = 0; i < vars_.Length(); i++) {
            if (inits_.At(i)->EvalConstantExpr() == nullptr) {
                return nullptr;
            }
        }

        Object* last = nullptr;
        for (i = 0; i < nodes_.Length(); i++) {
            if ((last = nodes_.At(i)->EvalConstantExpr()) == nullptr) {
                return nullptr;
            }
        }

        return last;
    }

    bool LoadLocalNode::IsConstant() {
        return local_.IsConstant();
    }

    Object* LoadLocalNode::EvalConstantExpr() {
        if (local_.IsConstant()) {
            return local_.GetConstantValue();
        }

        return nullptr;
    }

    AstNode* LoadLocalNode::MakeAssignmentNode(AstNode* rhs) {
        return new StoreLocalNode(&local_, rhs);
    }

#define KIND TokenKind

    bool BinaryOpNode::IsConstant() {
        switch (op_) {
            case KIND::kOR:
            case KIND::kAND: {
                if (left_->IsLiteralNode() &&
                    left_->AsLiteralNode()->Literal()->IsNil()) {
                    return false;
                }

                if (right_->IsLiteralNode() &&
                    right_->AsLiteralNode()->Literal()->IsNil()) {
                    return false;
                }

                // Fallthrough
            }
            case KIND::kADD:
            case KIND::kSUB:
            case KIND::kMUL:
            case KIND::kDIV: {
                return left_->IsConstant() && right_->IsConstant();
            }
            default:
                return false;
        }
    }

    Object* BinaryOpNode::EvalConstantExpr() {
        Object* left = left_->EvalConstantExpr();
        if (left == nullptr) {
            return nullptr;
        }

        if (!IS_NUMBER(left) && !IS_BOOLEAN(left) && !IS_STRING(left)) {
            return nullptr;
        }

        Object* right = right_->EvalConstantExpr();
        if (right == nullptr) {
            return nullptr;
        }

        switch (op_) {
            case KIND::kADD:
            case KIND::kSUB:
            case KIND::kMUL:
            case KIND::kDIV: {
                if (IS_NUMBER(left) && IS_NUMBER(right)) {
                    return left;
                }

                return nullptr;
            }
            default:
                return nullptr;
        }
    }

    void StoreLocalNode::EmitInstructions(std::vector<Instruction*>* instrs) {
        instrs->push_back(new StoreLocalInstr(&local_));
    }

    void LoadLocalNode::EmitInstructions(std::vector<Instruction*>* instrs) {
        instrs->push_back(new LoadLocalInstr(TMP, &local_));
    }

    void ReturnNode::EmitInstructions(std::vector<Instruction*>* instrs) {
        instrs->push_back(new ReturnInstr);
    }

    void BinaryOpNode::EmitInstructions(std::vector<Instruction*>* instrs) {}
    void LocalNode::EmitInstructions(std::vector<Instruction*>* instrs) {}
    void CompoundCallNode::EmitInstructions(std::vector<Instruction*>* instrs) {}
    void LiteralNode::EmitInstructions(std::vector<Instruction*>* instrs) {}
}