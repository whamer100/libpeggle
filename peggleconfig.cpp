#include <fstream>

#include "libpeggle.h"
#include "utils.h"

namespace Peggle {

#pragma region libpeggle_Config

    struct Token {
        union TokenData {  // only one can be possible at a time, so this is ok
            std::string* String;
            int32_t Integer;
            float_t Decimal;
        };
    private:
        TokenType Type;
        TokenData Data{};

    public:
        explicit Token(const TokenType type, const std::string& data) {
            Type = type;
            if (type == TokenType::String)
                Data.String = new std::string(data);
            else if (type == TokenType::Integer)
                Data.Integer = std::stoi(data);
            else if (type == TokenType::Decimal)
                Data.Decimal = std::stof(data);
            else
                throw std::exception("Attempted to construct a token with no type!");
        }

        explicit Token(const std::string& data) {
            Type = TokenType::String;
            Data.String = new std::string(data);
        }
        explicit Token(const int32_t data) {
            Type = TokenType::Integer;
            Data.Integer = data;
        }
        explicit Token(const float_t data) {
            Type = TokenType::Decimal;
            Data.Decimal = data;
        }

        void Update(const std::string& data) {
            if (Type == TokenType::String)
                Data.String->operator=(data);
            else {
                Type = TokenType::String;
                Data.String = new std::string(data);
            }
        }
        void Update(const int32_t data) {
            if (Type == TokenType::String)
                delete Data.String;
            Data.Integer = data;
        }
        void Update(const float_t data) {
            if (Type == TokenType::String)
                delete Data.String;
            Data.Decimal = data;
        }

        static const std::string* GetTokenStringRef(const Token* token) {
            if (token->Type != TokenType::String)
                return nullptr;
            return token->Data.String;
        }
        static const std::string* GetTokenStringRef(const Token& token) {
            if (token.Type != TokenType::String)
                return nullptr;
            return token.Data.String;
        }
        static std::string GetTokenString(const Token* token) {
            if (token->Type != TokenType::String)
                return "";
            return *token->Data.String;
        }
        static std::string GetTokenString(const Token& token) {
            if (token.Type != TokenType::String)
                return "";
            return *token.Data.String;
        }

        static int32_t GetTokenInteger(const Token& token) {
            if (token.Type != TokenType::Integer)
                return 0;
            return token.Data.Integer;
        }
        static int32_t GetTokenInteger(const Token* token) {
            if (token->Type != TokenType::Integer)
                return 0;
            return token->Data.Integer;
        }

        static float_t GetTokenDecimal(const Token& token) {
            if (token.Type != TokenType::Decimal)
                return 0;
            return token.Data.Decimal;
        }
        static float_t GetTokenDecimal(const Token* token) {
            if (token->Type != TokenType::Decimal)
                return 0;
            return token->Data.Decimal;
        }

        static TokenType GetType(const Token& token) {
            return token.Type;
        }
        static TokenType GetType(const Token* token) {
            return token->Type;
        }

        ~Token() {
            if (Type == TokenType::String) {
                delete Data.String;
            }
        }
    };

    ConfigTypes::StageCfg Config::LoadStageConfig(const std::string& cfg_string) {
        std::string cfg_clean = Utils::remove_comments(cfg_string);
        Utils::strip_inplace(cfg_clean);
        ConfigTypes::StageCfg cfg{};
        const auto lines = Utils::split_string(cfg_clean);
        ConfigTypes::Stage ctx{};
        for (const auto& line : lines) {
            if (line.empty() || line == "{")
                continue;

            if (line == "}")
                cfg.Stages.emplace_back(ctx);
            else if (line == "Stage") {
                ctx = {};
            }
            else if (line.starts_with("Level")) {
                const auto tokens = Tokenize(Utils::split_to(line, ':'));
                if (tokens.size() == 1)
                    ctx.Levels.emplace_back(GetTokenString(tokens.front()), GetTokenString(tokens.front()));
                else if (tokens.size() != 2)
                    return {};  // invalid state
                else
                    ctx.Levels.emplace_back(GetTokenString(tokens.front()), GetTokenString(tokens.back()));
            }
            else if (line.starts_with("Dialog")) {
                const auto tokens = Tokenize(Utils::split_to(line, ':'));
                if (tokens.size() == 2) {
                    ctx.Dialog.emplace_back(
                        GetTokenInteger(tokens[0]),
                        GetTokenString(tokens[1])
                    );
                }
                else if (tokens.size() != 3)
                    return {};  // invalid state
                else
                    ctx.Dialog.emplace_back(
                        GetTokenInteger(tokens[0]),
                        GetTokenString(tokens[1]),
                        GetTokenString(tokens[2])
                    );
            }
            else if (line.starts_with("StageDialog")) {
                const auto tokens = Tokenize(Utils::split_to(line, ':'));
                if (tokens.size() != 2)
                    return {};
                ctx.StageDialog.emplace_back(GetTokenInteger(tokens[0]), GetTokenString(tokens[1]));
            }
            else if (line.starts_with("Credit")) {  // always a part of the final stage
                const auto tokens = Tokenize(Utils::split_to(line, ':'));
                if (tokens.size() == 2)
                    ctx.Credits.emplace_back(
                        GetTokenInteger(tokens[0]),
                        GetTokenString(tokens[1]),
                        -1
                    );
                else if (tokens.size() == 3)
                    ctx.Credits.emplace_back(
                        GetTokenInteger(tokens[0]),
                        GetTokenString(tokens[1]),
                        GetTokenInteger(tokens[2])
                    );
                else
                    return {};  // invalid state
            }
            else if (line.starts_with("ExcludeRandStages")) {
                const auto tokens = Tokenize(Utils::split_to(line, ':'));
                for (const auto& tok : tokens) {
                    cfg.ExcludeRandStages.emplace_back(GetTokenInteger(tok));
                }
            }
            else if (line.starts_with("IncludeRandLevels")) {
                const auto tokens = Tokenize(Utils::split_to(line, ':'));
                for (const auto& tok : tokens) {
                    cfg.IncludeRandLevels.emplace_back(GetTokenString(tok));
                }
            }
            else if (line.starts_with("Tip")) {
                const auto tokens = Tokenize(Utils::split_to(line, ':'));
                cfg.Tips.emplace_back(GetTokenString(tokens.front()));
            }
        }

        cfg.Valid = true;
        return cfg;
    }
    ConfigTypes::TrophyCfg Config::LoadTrophyConfig(const std::string& cfg_string) {
        std::string cfg_clean = Utils::remove_comments(cfg_string);
        Utils::strip_inplace(cfg_clean);
        ConfigTypes::TrophyCfg cfg{};
        const auto lines = Utils::split_string(cfg_clean);
        ConfigTypes::Page p_ctx{};
        ConfigTypes::Trophy t_ctx{};
        bool in_trophy = false;
        for (const auto& line : lines) {
            if (line.empty() || line == "{")
                continue;

            if (line == "}") {
                if (in_trophy) {
                    p_ctx.Trophies.emplace_back(t_ctx);
                    in_trophy = false;
                } else {
                    cfg.Pages.emplace_back(p_ctx);
                }
            }
            else if (line.starts_with("Page")) {
                const auto tokens = Tokenize(Utils::split_to_first_whitespace(line));
                p_ctx = {};
                if (tokens.empty())
                    p_ctx.Name = "";
                else
                    p_ctx.Name = GetTokenString(tokens.front());
            }
            else if (line.starts_with("Trophy")) {
                const auto tokens = Tokenize(Utils::split_to_first_whitespace(line));
                in_trophy = true;
                t_ctx = {};
                if (tokens.empty())
                    t_ctx.Name = "";
                else
                    t_ctx.Name = GetTokenString(tokens.front());
            } else if (line.starts_with("Id")) {
                const auto tokens = Tokenize(Utils::split_to(line, ':'));
                if (in_trophy) {
                    t_ctx.Id = GetTokenInteger(tokens.front());
                }
            } else if (line.starts_with("Desc")) {
                const auto tokens = Tokenize(Utils::split_to(line, ':'));
                if (in_trophy) {
                    t_ctx.Etc.emplace_back("Desc", tokens);
                } else {
                    p_ctx.Desc = GetTokenString(tokens.front());
                }
            } else if (line.starts_with("SmallDesc")) {
                const auto tokens = Tokenize(Utils::split_to(line, ':'));
                if (in_trophy) {
                    t_ctx.Etc.emplace_back("SmallDesc", tokens);
                } else {
                    p_ctx.SmallDesc = GetTokenString(tokens.front());
                }
            } else if (in_trophy && line.find(':') != std::string::npos) {
                const auto key = Utils::scan_to(line, ':');
                const auto tokens = Tokenize(Utils::split_to(line, ':'));
                t_ctx.Etc.emplace_back(key, tokens);
            }
        }

        cfg.Valid = true;
        return cfg;
    }
    ConfigTypes::CharacterCfg Config::LoadCharacterConfig(const std::string& cfg_string) {
        std::string cfg_clean = Utils::remove_comments(cfg_string);
        Utils::strip_inplace(cfg_clean);
        ConfigTypes::CharacterCfg cfg{};
        const auto lines = Utils::split_string(cfg_clean);
        ConfigTypes::Character ctx{};
        for (const auto& line : lines) {
            if (line.empty() || line == "{")
                continue;

            if (line == "}")
                cfg.Characters.emplace_back(ctx);
            else if (line.starts_with("Character")) {
                const auto tokens = Tokenize(Utils::split_to(line, ' '));
                ctx = {};
                if (tokens.empty())
                    ctx.Name = "";
                else
                    ctx.Name = GetTokenString(tokens.front());
            }
            else if (line.starts_with("Powerup")) {
                const auto tokens = Tokenize(Utils::split_to(line, ':'));
                ctx.Powerup = GetTokenString(tokens.front());
            }
            else if (line.starts_with("Desc")) {
                const auto tokens = Tokenize(Utils::split_to(line, ':'));
                ctx.Desc = GetTokenString(tokens.front());
            }
            else if (line.starts_with("Tip")) {
                const auto tokens = Tokenize(Utils::split_to(line, ':'));
                ctx.Tips.emplace_back(GetTokenString(tokens.front()));
            }
            else if (line.find(':') != std::string::npos) {
                const auto key = Utils::scan_to(line, ':');
                const auto tokens = Tokenize(Utils::split_to(line, ':'));
                ctx.Etc.emplace_back(key, tokens);
            }
        }

        cfg.Valid = true;
        return cfg;
    }

    ConfigTypes::StageCfg Config::LoadStageConfig(const std::filesystem::path& path) {
        if (!exists(path))
            return ConfigTypes::StageCfg{};  // Valid = false
        std::ifstream cfg_s(path, std::ifstream::in);
        const std::string cfg_string = Utils::slurp(cfg_s);
        cfg_s.close();
        return LoadStageConfig(cfg_string);
    }
    ConfigTypes::StageCfg Config::LoadStageConfig(const Pak& pak, const std::filesystem::path& path) {
        if (!pak.HasFile(path.generic_string()))
            return ConfigTypes::StageCfg{};  // Valid = false
        const auto cfg = pak.GetFile(path.generic_string());
        const std::string cfg_string((char*)cfg.Data, cfg.Size);
        return LoadStageConfig(cfg_string);
    }

    ConfigTypes::TrophyCfg Config::LoadTrophyConfig(const std::filesystem::path& path) {
        if (!exists(path))
            return ConfigTypes::TrophyCfg{};  // Valid = false
        std::ifstream cfg_s(path, std::ifstream::in);
        const std::string cfg_string = Utils::slurp(cfg_s);
        cfg_s.close();
        return LoadTrophyConfig(cfg_string);
    }
    ConfigTypes::TrophyCfg Config::LoadTrophyConfig(const Pak& pak, const std::filesystem::path& path) {
        if (!pak.HasFile(path.generic_string()))
            return ConfigTypes::TrophyCfg{};  // Valid = false
        const auto cfg = pak.GetFile(path.generic_string());
        const std::string cfg_string((char*)cfg.Data, cfg.Size);
        return LoadTrophyConfig(cfg_string);
    }

    ConfigTypes::CharacterCfg Config::LoadCharacterConfig(const std::filesystem::path& path) {
        if (!exists(path))
            return ConfigTypes::CharacterCfg{};  // Valid = false
        std::ifstream cfg_s(path, std::ifstream::in);
        const std::string cfg_string = Utils::slurp(cfg_s);
        cfg_s.close();
        return LoadCharacterConfig(cfg_string);
    }
    ConfigTypes::CharacterCfg Config::LoadCharacterConfig(const Pak& pak, const std::filesystem::path& path) {
        if (!pak.HasFile(path.generic_string()))
            return ConfigTypes::CharacterCfg{};  // Valid = false
        const auto cfg = pak.GetFile(path.generic_string());
        const std::string cfg_string((char*)cfg.Data, cfg.Size);
        return LoadCharacterConfig(cfg_string);
    }

    std::string Config::BuildConfig(const ConfigTypes::StageCfg& cfg) {
        return "";
    }
    std::string Config::BuildConfig(const ConfigTypes::TrophyCfg& cfg) {
        return "";
    }
    std::string Config::BuildConfig(const ConfigTypes::CharacterCfg& cfg) {
        return "";
    }

    void Config::SaveConfig(const ConfigTypes::StageCfg& cfg, Pak& pak, const std::filesystem::path& path) {
        const auto cfg_string = BuildConfig(cfg);
        pak.UpdateFile(path.generic_string(), cfg_string.c_str(), cfg_string.size());
    }
    void Config::SaveConfig(const ConfigTypes::TrophyCfg& cfg, Pak& pak, const std::filesystem::path& path) {
        const auto cfg_string = BuildConfig(cfg);
        pak.UpdateFile(path.generic_string(), cfg_string.c_str(), cfg_string.size());
    }
    void Config::SaveConfig(const ConfigTypes::CharacterCfg& cfg, Pak& pak, const std::filesystem::path& path) {
        const auto cfg_string = BuildConfig(cfg);
        pak.UpdateFile(path.generic_string(), cfg_string.c_str(), cfg_string.size());
    }

    TokenType Config::GetTokenType(const Token& token) {
        return Token::GetType(token);
    }
    TokenType Config::GetTokenType(const Token* token) {
        return Token::GetType(token);
    }

    const std::string* Config::GetTokenStringRef(const Token* token) {
        return Token::GetTokenStringRef(token);
    }
    const std::string* Config::GetTokenStringRef(const Token& token) {
        return Token::GetTokenStringRef(token);
    }
    std::string Config::GetTokenString(const Token* token) {
        return Token::GetTokenString(token);
    }
    std::string Config::GetTokenString(const Token& token) {
        return Token::GetTokenString(token);
    }

    int32_t Config::GetTokenInteger(const Token& token) {
        return Token::GetTokenInteger(token);
    }
    int32_t Config::GetTokenInteger(const Token* token) {
        return Token::GetTokenInteger(token);
    }

    float_t Config::GetTokenDecimal(const Token& token) {
        return Token::GetTokenDecimal(token);
    }
    float_t Config::GetTokenDecimal(const Token* token) {
        return Token::GetTokenDecimal(token);
    }

    void Config::UpdateToken(Token& token, const std::string& data) {
        token.Update(data);
    }
    void Config::UpdateToken(Token& token, const int32_t data) {
        token.Update(data);
    }
    void Config::UpdateToken(Token& token, const float_t data) {
        token.Update(data);
    }

    std::vector<Token*> Config::Tokenize(const std::string& text) {
        std::vector<Token*> tokens;
        std::string tmp;
        auto t = TokenType::Unset;
        bool esc = false;
        bool inStr = false;
        for (auto& ch : text) {
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
                tokens.emplace_back(new Token{t, tmp});
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
            tokens.emplace_back(new Token{t, tmp});

        return tokens;
    }

#pragma endregion

}
