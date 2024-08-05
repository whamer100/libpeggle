#include "libpeggle.h"

namespace Peggle {

#pragma region libpeggle_Config

    enum class TokenType {
        Unset,
        String,
        Integer,
        Decimal
    };

    struct Token {
        TokenType Type;
        union {  // only one can be possible at a time, so this is ok
            std::string* String;
            int32_t Integer;
            float_t Decimal;
        };

        explicit Token(const TokenType type) {
            Type = type;
            String = nullptr;
        }
        ~Token() {
            if (Type == TokenType::String) {
                delete String;
            }
        }
    };

    Config::Config(const std::filesystem::path &path) {

    }

    Config::Config(const Pak &pak, const std::filesystem::path &path) {

    }

    // TODO: test this
    std::vector<Token> Config::Tokenize(const std::string& cfg) {
        std::vector<Token> tokens;
        std::string tmp;
        auto t = TokenType::Unset;
        bool esc = false;
        bool inStr = false;
        for (auto& ch : cfg) {
            if (esc) {
                tmp += ch;
                esc = false;
                continue;
            }
            if (ch == '\\')
                esc = true;
            if (t == TokenType::String) {
                if (ch == '"') {
                    inStr = false;
                    continue;
                }
            }
            if (ch == ',' && !inStr) {
                if (t == TokenType::Unset)
                    t = TokenType::String;
                ConstructToken(t, tmp);
                t = TokenType::Unset;
                tmp = "";
                continue;
            }
            if (t == TokenType::Integer and ch == '.')
                t = TokenType::Decimal;  // this exists in like, very rare cases lmao
            if (t == TokenType::Unset) {
                if (ch == ' ' || ch == '\t')
                    continue;  // skip whitespace between tokens
                if (std::isdigit(ch) or ch == '-')
                    t = TokenType::Integer;
                if (ch == '"') {
                    t = TokenType::String;
                    inStr = true;
                    continue;
                }
                if (std::isalpha(ch))
                    t = TokenType::String;
            }
            if (t == TokenType::Integer && !std::isdigit(ch))
                t = TokenType::String;  // object just started with a number
            tmp += ch;
        }
        if (!tmp.empty() || t != TokenType::Unset)
            ConstructToken(t, tmp);

        return tokens;
    }

    // TODO: test this
    Token Config::ConstructToken(const TokenType type, const std::string& data) {
        auto token = Token(type);

        if (type == TokenType::String)
            token.String = new std::string(data);
        else if (type == TokenType::Integer)
            token.Integer = std::stoi(data);
        else if (type == TokenType::Decimal)
            token.Decimal = std::stof(data);
        else
            throw std::exception("Attempted to construct a token with no type!");

        // this is okay to ignore since its a union, setting one fixes the rest
        // ReSharper disable once CppSomeObjectMembersMightNotBeInitialized
        return token;
    }

#pragma endregion

}