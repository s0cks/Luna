#include "parser.h"
#include "intermediate_language.h"

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
               c == '=';
    }

    bool Parser::IsKeyword(std::string str) {
        return keywords_.find(str) != keywords_.end();
    }

    void Parser::PushBack(Token* token) {
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
            case '=': return new Token(TokenKind::kEQUALS, "=");
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

    Function* Parser::Parse(Scope* scope) {
        Function* func = new Function("___main___");
        Token* next;
        while((next = NextToken())->GetKind() != TokenKind::kEOF){
            switch(next->GetKind()){
                case TokenKind::kLOCAL:{
                    std::string ident = ((next = NextToken())->GetText());
                    if((next = NextToken())->GetKind() != TokenKind::kEQUALS){
                        std::cerr << "Unexpected " << next->GetText() << std::endl;
                        abort();
                    }

                    switch((next = NextToken())->GetKind()){
                        case TokenKind::kLIT_STRING:{
                            scope->Define(ident, new String(next->GetText()));
                            break;
                        }
                    }

                    break;
                }
                case TokenKind::kIDENTIFIER:{
                    std::string ident = next->GetText();
                    if((next = NextToken())->GetKind() == TokenKind::kLPAREN){
                        while((next = NextToken())->GetKind() != TokenKind::kRPAREN){
                            switch(next->GetKind()){
                                case TokenKind::kLIT_STRING:{
                                    func->instructions_.push_back(new PushArgumentInstr(new String(next->GetText())));
                                    break;
                                }
                                case TokenKind::kLIT_NUMBER:{
                                    func->instructions_.push_back(new PushArgumentInstr(new Number(atof(next->GetText().c_str()))));
                                    break;
                                }
                                case TokenKind::kIDENTIFIER:{
                                    std::string ident2 = next->GetText();
                                    if((next = NextToken())->GetKind() == TokenKind::kLBRACKET){
                                        Object* obj = scope->Lookup(ident2);
                                        if(obj == nullptr || obj->Type() != kTableTID){
                                            std::cerr << ident2 << " not a table" << std::endl;
                                            abort();
                                        }
                                        Table* tbl = static_cast<Table*>(obj);
                                        if((next = NextToken())->GetKind() != TokenKind::kLIT_NUMBER){
                                            std::cerr << next->GetText() << "[" << next->GetKind() << "] not an indexable object" << std::endl;
                                            abort();
                                        }

                                        func->instructions_.push_back(new LoadObjectInstr(RAX, tbl));
                                        func->instructions_.push_back(new GetTableEntryInstr(RAX, atoi(next->GetText().c_str()), RCX));
                                        func->instructions_.push_back(new PushArgumentInstr(RCX));
                                    } else{
                                        PushBack(next);
                                        func->instructions_.push_back(new LoadObjectInstr(RAX, scope->Lookup(ident2)));
                                        func->instructions_.push_back(new PushArgumentInstr(RAX));
                                    }

                                    break;
                                }
                                default:{
                                    std::cerr << "Unexpected " << next->GetText() << "[lookahead: " << (next = NextToken())->GetText() << "]" << std::endl;
                                    abort();
                                }
                            }
                        }

                        Object* potFunc = scope->Lookup(ident);
                        if(potFunc == nullptr || !potFunc->IsExecutable()){
                            std::cerr << ident << " not a function!" << std::endl;
                            abort();
                        }
                        Function* f = static_cast<Function*>(potFunc);
                        if(f->IsNative()){
                            func->instructions_.push_back(new NativeCallInstr(f));
                        } else{
                            //TODO: CompoundCallInstr
                        }
                    }

                    break;
                }
                default:{
                    std::cerr << "Unexpected: " << next->GetText() << "[" << next->GetKind() << "]" << std::endl;
                    abort();
                }
            }
        }

        func->instructions_.push_back(new ReturnInstr);
        return func;
    }
}