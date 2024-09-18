#ifndef UTILS_H
#define UTILS_H
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>

namespace Utils {
    // https://stackoverflow.com/a/13172514
    inline std::vector<std::string> split_string(const std::string& str,
                                             const std::string& delimiter)
    {
        std::vector<std::string> strings;

        std::string::size_type pos = 0;
        std::string::size_type prev = 0;
        while ((pos = str.find(delimiter, prev)) != std::string::npos)
        {
            strings.push_back(str.substr(prev, pos - prev));
            prev = pos + delimiter.size();
        }

        // To get the last substring (or only, if delimiter is not found)
        strings.push_back(str.substr(prev));

        return strings;
    }
    inline std::vector<std::string> split_string(const std::string& str) {
        return split_string(str, "\n");
    }

    // https://stackoverflow.com/a/116220/3478582  // i thought this one had a funny name, and it did what i wanted lmao
    inline std::string slurp(const std::ifstream& in) {
        std::ostringstream sstr;
        sstr << in.rdbuf();
        return sstr.str();
    }

    inline std::string strip(const std::string& str) {
        const std::regex trim_whitespace(R"(^[ \t]+|[ \t]+$)");
        const auto str_stripped = std::regex_replace(str, trim_whitespace, "");
        return str_stripped;
    }

    inline void strip_inplace(std::string& str) {
        const std::regex trim_whitespace(R"(^[ \t]+|[ \t]+$)");
        str = std::regex_replace(str, trim_whitespace, "");
    }

    inline std::string remove_comments(const std::string& str) {
        const std::regex remove_comments(
            R"(((["'])(?:\\[\s\S]|.)*?\2|\/(?![*\/])(?:\\.|\[(?:\\.|.)\]|.)*?\/)|\/\/.*?$|\/\*[\s\S]*?\*\/)"
            );
        std::string res = std::regex_replace(str, remove_comments, "$1");
        return res;
    }

    inline std::string split_to_first_whitespace(const std::string& str) {
        constexpr char whitespace[2] = {' ', '\t'};
        size_t offset = 0;
        for (const char ch : str) {
            for (const char w : whitespace) {
                if (ch == w)
                    return str.substr(offset + 1, str.length() - offset + 1);
            }
            ++offset;
        }
    }

    inline std::string split_to(const std::string& str, const std::string& delimiter) {
        const auto offset = str.find_first_of(delimiter);
        if (offset == std::string::npos)
            return str;  // delimiter not found, return full string
        return str.substr(offset + 1, str.length() - offset + delimiter.size());
    }

    inline std::string split_to(const std::string& str, const char delimiter) {
        const auto offset = str.find_first_of(delimiter);
        if (offset == std::string::npos)
            return str;  // delimiter not found, return full string
        return str.substr(offset + 1, str.length() - offset + 1);
    }

    inline std::string scan_to(const std::string& str, const std::string& delimiter) {
        const auto offset = str.find_first_of(delimiter);
        if (offset == std::string::npos)
            return str;  // delimiter not found, return full string
        return str.substr(0, offset);
    }

    inline std::string scan_to(const std::string& str, const char delimiter) {
        const auto offset = str.find_first_of(delimiter);
        if (offset == std::string::npos)
            return str;  // delimiter not found, return full string
        return str.substr(0, offset);
    }
}

#endif //UTILS_H
