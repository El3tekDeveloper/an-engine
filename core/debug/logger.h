#pragma once
#ifndef ENGINE_DEBUG_ENABLED
#define ENGINE_DEBUG_ENABLED 1
#endif

#include <fmt/core.h>
#include <fmt/color.h>
#include <tuple>

namespace Logger {
    enum LogLevel {
        LOG_INFO,
        LOG_WARN,
        LOG_ERROR,
        LOG_ASSERT,
        LOG_SUCCESS,
        LOG_DEBUG
    };

    namespace detail {
        template<typename T>
        decltype(auto) make_printable(T&& val) {
            using Base = std::remove_cv_t<std::remove_pointer_t<std::remove_reference_t<T>>>;
            if constexpr (std::is_pointer_v<std::remove_reference_t<T>> && !std::is_same_v<Base, char>)
                return static_cast<const void*>(val);
            else
                return std::forward<T>(val);
        }

        constexpr fmt::color level_color(LogLevel level) noexcept {
            switch (level) {
                case LOG_INFO:    return fmt::color::pale_green;
                case LOG_WARN:    return fmt::color::light_yellow;
                case LOG_ERROR:   return fmt::color::dark_red;
                case LOG_ASSERT:  return fmt::color::dark_red;
                case LOG_SUCCESS: return fmt::color::light_green;
                case LOG_DEBUG:   return fmt::color::dark_magenta;
                default:          return fmt::color::white;
            }
        }

        constexpr const char* level_label(LogLevel level) noexcept {
            switch (level) {
                case LOG_INFO:    return "INFO";
                case LOG_WARN:    return "WARN";
                case LOG_ERROR:   return "ERROR";
                case LOG_ASSERT:  return "ASSERT";
                case LOG_SUCCESS: return "SUCCESS";
                case LOG_DEBUG:   return "DEBUG";
                default:          return "UNKNOWN";
            }
        }
    }

    template<typename... Args>
    void engine_log(LogLevel level, const char* file, int line,
                    fmt::string_view fmt_str, Args&&... args)
    {
#if ENGINE_DEBUG_ENABLED
        auto prefix = fmt::format(fmt::fg(detail::level_color(level)) | fmt::emphasis::bold,
                                  "[{}]", detail::level_label(level));

        if (level == LOG_ERROR || level == LOG_WARN || level == LOG_ASSERT)
            fmt::print(stderr, "{} ({}:{}) ", prefix, file, line);
        else
            fmt::print(stderr, "{} ", prefix);

        auto converted = std::make_tuple(detail::make_printable(std::forward<Args>(args))...);
        std::apply([&](auto&... c_args) {
            fmt::vprint(stderr, fmt_str, fmt::make_format_args(c_args...));
        }, converted);

        fmt::print(stderr, "\n");
#else
        (void)level; (void)file; (void)line; (void)fmt_str;
#endif
    }
}

#define LOG_INFO(fmt, ...)    Logger::engine_log(Logger::LOG_INFO,    __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)    Logger::engine_log(Logger::LOG_WARN,    __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)   Logger::engine_log(Logger::LOG_ERROR,   __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ASSERT(fmt, ...)  Logger::engine_log(Logger::LOG_ASSERT,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_SUCCESS(fmt, ...) Logger::engine_log(Logger::LOG_SUCCESS, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...)   Logger::engine_log(Logger::LOG_DEBUG,   __FILE__, __LINE__, fmt, ##__VA_ARGS__)
