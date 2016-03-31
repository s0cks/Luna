#include "parser.h"
#include "intermediate_language.h"
#include "compiler.h"
#include "ast.h"

namespace Luna{
    std::map<std::string, TokenKind> Parser::keywords_ = std::map<std::string, TokenKind>();

    Parser::Parser(FILE* file):
            peek_(nullptr),
            file_(file){
#define DEF_KW(val, name, desc) keywords_[name] = TokenKind::val;
        TOKENS_LIST(DEF_KW)
#undef DEF_KW
    }

    bool Parser::IsWhitespace(char c) {
        return c == '\t' ||
               c == '\n' ||
               c == '\r' ||
               c == ' ';
    }

    char Parser::Peek(){
        int c = getc(file_);
        ungetc(c, file_);
        return (char) c;
    }

    char Parser::Next(){
        int c = getc(file_);
        return (char) (c == EOF ? '\0' : c);
    }

    char Parser::NextReal() {
        char next;
        while(IsWhitespace(next = Next()));
        return next;
    }

    Token* Parser::ParseString(){
        char next;
        std::string buffer;
        while((next = Next()) != '"'){
            buffer += next;
        }
        return new Token(TokenKind::kLIT_STRING, buffer);
    }

    Token* Parser::ParseNumber(char next) {
        std::string buffer;
        buffer += next;
        while(isdigit(next = Peek()) || next == '.'){
            buffer += next;
            Next();
        }

        return new Token(TokenKind::kLIT_NUMBER, buffer);
    }

    bool Parser::IsSymbol(char c){
        return c == '(' ||
               c == ')' ||
               c == '[' ||
               c == ']' ||
               c == '{' ||
               c == '}' ||
               c == '=' ||
               c == ',';
    }

    bool Parser::IsKeyword(std::string str) {
        return keywords_.find(str) != keywords_.end();
    }

    void Parser::PushBack(Token* token) {
        std::cout << "Returning " << token->GetText() << std::endl;
        peek_ = token;
    }

    Token* Parser::NextToken() {
        if(peek_ != nullptr){
            Token* token = peek_;
            peek_ = nullptr;
            return token;
        }

        char next = NextReal();
        switch(next){
            case '(': return new Token(TokenKind::kLPAREN, "(");
            case ')': return new Token(TokenKind::kRPAREN, ")");
            case '[': return new Token(TokenKind::kLBRACKET, "[");
            case ']': return new Token(TokenKind::kRBRACKET, "]");
            case '{': return new Token(TokenKind::kLBRACE, "{");
            case '}': return new Token(TokenKind::kRBRACE, "}");
            case '=': return new Token(TokenKind::kEQUALS, "=");
            case ',': return new Token(TokenKind::kCOMMA, ",");
            case '\0': return new Token(TokenKind::kEOF, "");
        }

        if(next == '"'){
            return ParseString();
        } else if(isdigit(next)){
            return ParseNumber(next);
        } else if(next == EOF){
            return new Token(TokenKind::kEOF, "");
        } else{
            std::string buffer;
            buffer += next;
            while(!IsWhitespace(next = Peek()) && !IsSymbol(next)){
                buffer += next;
                if(IsKeyword(buffer)){
                    Next();
                    return new Token(keywords_.find(buffer)->second, buffer);
                }
                Next();
            }

            return new Token(TokenKind::kIDENTIFIER, buffer);
        }
    }

#define KIND TokenKind
#define CONSUME next = NextToken()

    Function* Parser::Parse(Scope* scope) {
        Function* func = new Function("__main__", scope);

        Token* next;
        while((next = NextToken())->GetKind() != KIND::kEOF){
            std::cout << next->GetText() << std::endl;
            switch(next->GetKind()){
                case KIND::kLOCAL:{
                    AstNode* init = ParseLocalDefinition(scope);
                    func->ast_.push_back(init);
                    break;
                }
                case KIND::kRETURN:{
                    std::cout << "Parsing Return" << std::endl;
                    AstNode* retVal = ParseBinaryExpr(scope);
                    func->ast_.push_back(new ReturnNode(retVal));
                    break;
                }
                default:{
                    std::cerr << "Unexpected " << next->GetText() << std::endl;
                    abort();
                }
            }
        }

        Compiler::Compile(func);
        return func;
    }

    bool Parser::ResolveIdentifierInLocalScope(Scope* scope, std::string ident, AstNode** node) {
        LocalVariable* local = scope->LookupVariable(ident);
        if(local != nullptr){
            if(node != nullptr){
                *node = new LoadLocalNode(local);
            }

            return true;
        }

        //TODO: Check for top-level-ness
        if(node != nullptr){
            *node = nullptr;
        }
        return false;
    }

    AstNode* Parser::ParseUnaryExpr(Scope* scope){
        AstNode* primary = nullptr;
        Token* next = NextToken();
        std::cout << "ParseUnaryExpr: " << next->GetText() << std::endl;

        switch((next)->GetKind()){
            case KIND::kIDENTIFIER:{
                std::string ident = next->GetText();
                CONSUME;
                if(!ResolveIdentifierInLocalScope(scope, ident, &primary)){
                    std::cerr << "Couldn't resolve " << ident << std::endl;
                    abort();
                }
                break;
            }
            case KIND::kLIT_NUMBER:{
                primary = new LiteralNode(new Number(atof(next->GetText().c_str())));
                break;
            }
            case KIND::kLIT_STRING:{
                primary = new LiteralNode(new String(next->GetText()));
                break;
            }
            case KIND::kTRUE:{
                primary = new LiteralNode(Boolean::TRUE);
                break;
            }
            case KIND::kFALSE:{
                primary = new LiteralNode(Boolean::FALSE);
                break;
            }
            case KIND::kNIL:{
                primary = new LiteralNode(Object::NIL);
                break;
            }
            default:{
                std::cerr << "Unexpected " << next->GetText() << std::endl;
                abort();
            }
        }

        return primary;
    }

    static bool IsValidExprToken(Token* token){
        TokenKind kind = token->GetKind();
        return kind == KIND::kLIT_STRING ||
               kind == KIND::kLIT_NUMBER ||
               kind == KIND::kIDENTIFIER ||
               kind == KIND::kAND ||
               kind == KIND::kOR ||
               kind == KIND::kFALSE ||
               kind == KIND::kTRUE ||
               kind == KIND::kNIL ||
               kind == KIND::kNOT;
    }

    AstNode* Parser::ParseBinaryExpr(Scope* scope) {
        AstNode* left = ParseUnaryExpr(scope);
        Token* next = NextToken();
        std::cout << "ParseBinaryExpr: " << next->GetText() << std::endl;
        if(IsValidExprToken(next)){
            while(IsValidExprToken(next)){
                TokenKind op = next->GetKind();
                AstNode* right = ParseBinaryExpr(scope);
                left = new BinaryOpNode(op, left, right);
                next = NextToken();
            }
        }
        return left;
    }

    AstNode* Parser::ParseLocalDefinition(Scope* scope) {
        Token* next;
        if((next = NextToken())->GetKind() != KIND::kIDENTIFIER){
            std::cerr << "Expected identifier, but got " << next->GetText() << std::endl;
            abort();
        }

        std::string ident = next->GetText();

        AstNode* init = nullptr;
        LocalVariable* var = nullptr;

        if((next = NextToken())->GetKind() == KIND::kEQUALS){
            AstNode* expr = ParseBinaryExpr(scope);
            var = new LocalVariable(ident, kUnknownTID);
            init = new StoreLocalNode(var, expr);
            if(expr->IsLiteralNode()){
                var->SetConstantValue(expr->AsLiteralNode()->Literal());
            }
        } else{
            PushBack(next);
            var = new LocalVariable(ident, kUnknownTID);
            AstNode* nullexpr = new LiteralNode(Object::NIL);
            init = new StoreLocalNode(var, nullexpr);
        }

        scope->AddVariable(var);
        return init;
    }
}