#include "type_registry.h"
#include <string>
#include <typeindex>
#include <unordered_map>
#include <charconv>

template<typename T>
std::string to_string_data_type(const void* p) {
    return std::to_string(*static_cast<const T*>(p));
}

template <typename T>
bool from_string_data_type(std::string_view input, void* output_ptr) {
    T* typed_ptr = static_cast<T*>(output_ptr);
    auto result = std::from_chars(input.data(), input.data() + input.size(), *typed_ptr);
    return result.ec == std::errc();
}

std::unordered_map<std::type_index, Type*>& known_types() {
    static std::unordered_map<std::type_index, Type*> instance = [] {
        std::unordered_map<std::type_index, Type*> types;

        types.emplace(typeid(int), new Type{"int", sizeof(int), &to_string_data_type<int>, &from_string_data_type<int>});
        types.emplace(typeid(uint32_t), new Type{"uint32_t", sizeof(uint32_t), &to_string_data_type<uint32_t>, &from_string_data_type<uint32_t>});
        types.emplace(typeid(float), new Type{"float", sizeof(float), &to_string_data_type<float>, &from_string_data_type<float>});
        types.emplace(typeid(double), new Type{"double", sizeof(double), &to_string_data_type<double>, &from_string_data_type<double>});

        types.emplace(typeid(unsigned short), new Type{"unsigned short", sizeof(unsigned short),
            [](const void* p) -> std::string {
                return std::to_string(*static_cast<const unsigned short*>(p));
            },
            [](std::string_view str, void* p) -> bool {
                unsigned short value{};
                auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
                if (ec != std::errc())
                    return false;
                *static_cast<unsigned short*>(p) = value;
                return true;
            }
        });

        types.emplace(typeid(bool), new Type{"bool", sizeof(bool),
            [](const void* p) -> std::string {
                return std::to_string(*static_cast<const bool*>(p));
            },
            [](std::string_view str, void* p) -> bool {
                if (str == "1" || str == "true")  { *static_cast<bool*>(p) = true;  return true; }
                if (str == "0" || str == "false") { *static_cast<bool*>(p) = false; return true; }
                return false;
            }
        });

        types.emplace(typeid(char*), new Type{"char*", sizeof(char*),
            [](const void* p) -> std::string {
                const char* str = *static_cast<char* const*>(p);
                return str ? str : "";
            }
        });
        types.emplace(typeid(const char*), new Type{"const char*", sizeof(const char*),
            [](const void* p) -> std::string {
                const char* str = *static_cast<const char* const*>(p);
                return str ? str : "";
            }
        });
        types.emplace(typeid(std::string), new Type{"std::string", sizeof(std::string),
            [](const void* p) -> std::string {
                return *static_cast<const std::string*>(p);
            }
        });
        return types;
    }();
    return instance;
}
