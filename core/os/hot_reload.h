#pragma once
#include <dlfcn.h>
#include <link.h>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <deque>
#include <filesystem>
#include <format>
#include <functional>
#include <string>
#include <thread>
#include <vector>
#include "core/debug/logger.h"
#include "scene2/components/component_ops.h"
#include "tools/reflector/runtime/type_registry.h"

namespace fs = std::filesystem;

struct HotReloadDesc {
    fs::path library;
    fs::path sources;
    std::string build_command;
    std::vector<fs::path> restart_libraries;
};

class HotReload {
public:
    using Filter = std::function<bool(const fs::path&)>;

    std::function<bool()> on_load;
    std::function<void()> on_unload;

    explicit HotReload(HotReloadDesc desc) : desc(std::move(desc)) {
        if (!this->desc.library.empty())
            copies_dir = this->desc.library.parent_path() / "hot_reload" / this->desc.library.stem();
    }
    ~HotReload() { stop(); }

    HotReload(const HotReload&) = delete;
    HotReload& operator=(const HotReload&) = delete;

    void watch(const fs::path& root, std::function<void()> on_change, Filter filter = any_file) {
        watches.emplace_back(root, std::move(filter), std::move(on_change));
    }

    bool start() {
        if (!desc.library.empty()) {
            if (!fs::exists(desc.library)) return open(desc.library.filename());

            fs::create_directories(copies_dir);
            loaded_copy = next_copy();
            if (loaded_copy.empty() || !open(loaded_copy)) return false;
        }

        watcher = std::thread([this]() { run(); });
        return true;
    }

    void stop() {
        watching = false;
        if (watcher.joinable()) watcher.join();

        close();
        if (!copies_dir.empty()) {
            std::error_code ec;
            fs::remove_all(copies_dir, ec);
        }
    }

    void update() {
        if (library_changed.exchange(false)) reload();
        for (Watch& watch : watches)
            if (watch.changed.exchange(false)) watch.on_change();
    }

    bool reload() {
        const fs::path next = next_copy();
        if (next.empty()) return false;

        close();
        if (open(next)) {
            remove_copy(loaded_copy);
            loaded_copy = next;
            return true;
        }

        LOG_ERROR("HotReload: keeping the previous {}", desc.library.filename().string());
        remove_copy(next);
        open(loaded_copy);
        return false;
    }

    template<typename T>
    T symbol(const char* name) const {
        T found = reinterpret_cast<T>(dlsym(handle, name));
        if (!found) LOG_ERROR("HotReload: {}", dlerror());
        return found;
    }

    static bool any_file(const fs::path&) { return true; }

    static bool is_source(const fs::path& path) {
        const fs::path extension = path.extension();
        return extension == ".cpp" || extension == ".h" || extension == ".hpp" || path.filename() == "meson.build";
    }

private:
    struct Watch {
        Watch(fs::path root, Filter filter, std::function<void()> on_change)
            : root(std::move(root)), filter(std::move(filter)), on_change(std::move(on_change)) {}

        fs::path root;
        Filter filter;
        std::function<void()> on_change;
        size_t stamp = 0;
        std::atomic<bool> changed = false;
    };

    static constexpr auto poll_interval = std::chrono::milliseconds(200);

    HotReloadDesc desc;
    std::deque<Watch> watches;

    void* handle = nullptr;
    link_map* handle_map = nullptr;
    int generation = 0;
    fs::path copies_dir;
    fs::path loaded_copy;

    std::thread watcher;
    std::atomic<bool> watching = true;
    std::atomic<bool> library_changed = false;

    fs::path next_copy() {
        fs::path copy = copies_dir / std::format("{}.{}.so", desc.library.stem().string(), generation++);
        std::error_code ec;
        fs::copy_file(desc.library, copy, fs::copy_options::overwrite_existing, ec);
        if (ec) {
            LOG_ERROR("HotReload: cannot copy library: {}", ec.message());
            return {};
        }
        return copy;
    }

    static void remove_copy(const fs::path& copy) {
        std::error_code ec;
        fs::remove(copy, ec);
    }

    bool open(const fs::path& path) {
        handle = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
        if (!handle) {
            LOG_ERROR("HotReload: {}", dlerror());
            return false;
        }
        dlinfo(handle, RTLD_DI_LINKMAP, &handle_map);

        if (on_load && !on_load()) {
            close();
            return false;
        }
        return true;
    }

    void close() {
        if (!handle) return;
        if (on_unload) on_unload();

        std::erase_if(known_types(), [this](auto& type) { return is_inside_library(type.second); });
        std::erase_if(component_ops_registry(), [this](auto& ops) { return is_inside_library(ops.first); });

        dlclose(handle);
        handle = nullptr;
        handle_map = nullptr;
    }

    bool is_inside_library(const void* address) const {
        Dl_info info;
        link_map* owner = nullptr;
        return dladdr1(address, &info, reinterpret_cast<void**>(&owner), RTLD_DL_LINKMAP) && owner == handle_map;
    }

    static size_t stamp_of(const fs::path& root, const Filter& filter) {
        size_t stamp = 0;
        std::error_code ec;
        fs::recursive_directory_iterator it(root, ec), end;
        for (; !ec && it != end; it.increment(ec)) {
            std::error_code file_ec;
            if (!it->is_regular_file(file_ec) || !filter(it->path())) continue;
            const auto time = it->last_write_time(file_ec);
            if (!file_ec) stamp += time.time_since_epoch().count();
        }
        return stamp;
    }

    static fs::file_time_type write_time(const fs::path& path) {
        std::error_code ec;
        return fs::last_write_time(path, ec);
    }

    void build() {
        LOG_INFO("HotReload: building {}...", desc.library.filename().string());
        if (std::system(desc.build_command.c_str()) != 0)
            LOG_ERROR("HotReload: build failed, keeping what is running");
    }

    std::vector<fs::file_time_type> restart_times() const {
        std::vector<fs::file_time_type> times;
        for (const fs::path& library : desc.restart_libraries) times.push_back(write_time(library));
        return times;
    }

    void run() {
        const bool has_library = !desc.library.empty();
        const bool has_sources = has_library && !desc.sources.empty();

        for (Watch& watch : watches) watch.stamp = stamp_of(watch.root, watch.filter);
        size_t sources = has_sources ? stamp_of(desc.sources, is_source) : 0;
        fs::file_time_type library_time = has_library ? write_time(desc.library) : fs::file_time_type{};
        const std::vector<fs::file_time_type> restart_baseline = restart_times();

        while (watching) {
            std::this_thread::sleep_for(poll_interval);

            for (Watch& watch : watches) {
                const size_t current = stamp_of(watch.root, watch.filter);
                if (current != watch.stamp) {
                    watch.stamp = current;
                    watch.changed = true;
                }
            }

            if (!has_library) continue;

            if (has_sources) {
                const size_t current = stamp_of(desc.sources, is_source);
                if (current != sources) {
                    sources = current;
                    build();
                }
            }

            if (write_time(desc.library) == library_time) continue;

            std::this_thread::sleep_for(poll_interval);
            library_time = write_time(desc.library);

            if (restart_times() != restart_baseline)
                LOG_ERROR("HotReload: a dependency was rebuilt, restart the game to load this change");
            else
                library_changed = true;
        }
    }
};
