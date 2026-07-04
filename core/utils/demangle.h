#pragma once
#include <typeinfo>
#include <cxxabi.h>
#include <memory>
#include <string>

inline std::string demangle(const char* name) {
    int status = 0;
    std::unique_ptr<char, void(*)(void*)> result(
        abi::__cxa_demangle(name, nullptr, nullptr, &status),
        std::free
    );

    return status == 0 ? result.get() : name;
}

template<typename T>
std::string type_name() {
    return demangle(typeid(T).name());
}

template<typename T>
std::string type_name(const T& value) {
    return demangle(typeid(value).name());
}
