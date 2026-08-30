#pragma once
#include <sstream>
#include <string>
#include <vector>

inline std::string join(const std::vector<std::string>& strings, const std::string& separator) {
    std::string result;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i > 0)
            result += separator;

        result += strings[i];
    }
    return result;
}

inline std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    return result;
}

inline int nocasecmp_to(const std::string& a, const std::string& b) {
    size_t min_len = std::min(a.size(), b.size());
    for (size_t i = 0; i < min_len; ++i) {
        unsigned char ca = std::tolower(static_cast<unsigned char>(a[i]));
        unsigned char cb = std::tolower(static_cast<unsigned char>(b[i]));

        if (ca != cb)
            return ca < cb ? -1 : 1;
    }
    if (a.size() == b.size())
        return 0;
    return a.size() < b.size() ? -1 : 1;
}

const std::string WHITESPACE = " \n\r\t\f\v";
inline std::string trim(std::string s) {
    size_t start = s.find_first_not_of(WHITESPACE);
    size_t end = s.find_last_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

inline std::vector<std::string_view> split_top_level(std::string_view str) {
    std::vector<std::string_view> parts;
    size_t depth = 0;
    size_t start = 0;
    
    for (size_t i = 0; i < str.size(); ++i) {
        char c = str[i];
        if (c == '[') {
            ++depth;
        } else if (c == ']') {
            --depth;
        } else if (c == ',' && depth == 0) {
            parts.push_back(str.substr(start, i - start));
            start = i + 1;
        }
    }
    parts.push_back(str.substr(start));
    
    for (auto& p : parts) {
        while (!p.empty() && p.front() == ' ') p.remove_prefix(1);
        while (!p.empty() && p.back() == ' ')  p.remove_suffix(1);
    }
    return parts;
}

