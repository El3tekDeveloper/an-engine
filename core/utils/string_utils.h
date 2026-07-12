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
