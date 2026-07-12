#include "type_registry.h"
#include <string>
#include <typeindex>
#include <unordered_map>

std::unordered_map<std::type_index, Type> KNOWN_TYPES = {
    {typeid(int),      {"int", sizeof(int), &to_string_data_type<int>}},
    {typeid(uint32_t), {"uint32_t", sizeof(uint32_t), &to_string_data_type<uint32_t>}},
    {typeid(float),    {"float", sizeof(float), &to_string_data_type<float>}},
    {typeid(double),   {"double", sizeof(double), &to_string_data_type<double>}},

    {typeid(bool), {"bool", sizeof(bool),
        [](const void* p) -> std::string {
            return std::to_string(*static_cast<const bool*>(p));
        }
    }},

    {typeid(char*), {"char*", sizeof(char*),
        [](const void* p) -> std::string {
            const char* str = *static_cast<char* const*>(p);
            return str ? str : "";
        }
    }},
    {typeid(const char*), {"const char*", sizeof(const char*),
        [](const void* p) -> std::string {
            const char* str = *static_cast<const char* const*>(p);
            return str ? str : "";
        }
    }},
    {typeid(std::string), {"std::string", sizeof(std::string),
        [](const void* p) -> std::string {
            return *static_cast<const std::string*>(p);
        }
    }},
};
