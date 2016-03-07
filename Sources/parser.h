#ifndef LUNA_PARSER_H
#define LUNA_PARSER_H

#include <string>
#include <map>
#include "type.h"
#include "vm.h"

namespace Luna{
#define TOKENS_KEYWORDS_LIST(V) \
    V(kAND, "and", "<and>") \
    V(kBREAK, "break", "<break>") \
    V(kDO, "do", "<do>") \
    V(kELSE, "else", "<else>") \
    V(kELSEIF, "elseif", "<elseif>") \
    V(kEND, "end", "<end>") \
    V(kFALSE, "false", "<false>") \
    V(kFOR, "for", "<for>") \
    V(kFUNCTION, "function", "<function>") \
    V(kIF, "if", "<if>") \
    V(kIN, "in", "<in>") \
    V(kLOCAL, "local", "<local>") \
    V(kNIL, "nil", "<nil>") \
    V(kNOT, "not", "<not>") \
    V(kOR, "or", "<or>") \
    V(kREPEAT, "repeat", "<repeat>") \
    V(kRETURN, "return", "<return>") \
    V(kTHEN, "then", "<then>") \
    V(kTRUE, "true", "<true>") \
    V(kUNTIL, "until", "<until>") \
    V(kWHILE, "while", "<while>")

#define TOKENS_SYMBOLS_LIST(V) \
    V(kEQUALS, "=", "<equals>") \
    V(kLPAREN, "(", "<left parenthesis>") \
    V(kRPAREN, ")", "<right parenthesis>") \
    V(kLBRACKET, "[", "<left bracket>") \
    V(kRBRACKET, "]", "<right bracket>") \
    V(kLBRACE, "{", "<left brace>") \
    V(kRBRACE, "}", "<right brace>")

#define TOKENS_LITERALS_LIST(V) \
    V(kLIT_STRING, "", "<literal string>") \
    V(kLIT_NUMBER, "", "<literal long>")

#define TOKENS_LIST(V) \
    TOKENS_KEYWORDS_LIST(V) \
    TOKENS_LITERALS_LIST(V) \
    TOKENS_SYMBOLS_LIST(V)

    enum TokenKind{
#define DEF_TOKEN(token, name, desc) token,
        TOKENS_LIST(DEF_TOKEN)
#undef DEF_TOKEN
        kIDENTIFIER,
        kEOF
    };

    class Token{
    public:
        Token(TokenKind kind, std::string text):
                kind_(kind),
                text_(text){}

        TokenKind GetKind() const{
            return kind_;
        }

        std::string GetText() const{
            return text_;
        }
    private:
        TokenKind kind_;
        std::string text_;
    };

    class Parser{
    public:
        Parser(FILE* file);
        Function* Parse(Scope* scope);
    private:
        Token* peek_;
        FILE* file_;

        char Peek();
        char Next();
        char NextReal();
        Token* ParseString();
        Token* ParseNumber(char next);
        Token* NextToken();
        void PushBack(Token* token);

        static std::map<std::string, TokenKind> keywords_;
        static inline bool IsKeyword(std::string str);
        static inline bool IsWhitespace(char c);
        static inline bool IsSymbol(char c);
    };
}

#endif