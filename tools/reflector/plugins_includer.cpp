// gen_plugins.cpp
//
// Scans the source tree for reflector generator plugin headers and
// generates a single aggregator header that #includes all of them.
//
// This lets tools/reflector/plugin_main.cpp include one generated file
// instead of needing a manual #include added for every new plugin.
//
// Usage:
//   gen_plugins <source_root> <output_file>
//
// Portable: uses only the C++17 standard library (<filesystem>), so it
// builds and runs identically on Windows (MSVC/MinGW) and Linux/macOS.

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

bool has_suffix(const std::string& name, const std::string& suffix) {
    if (suffix.size() > name.size())
        return false;
    return std::equal(suffix.rbegin(), suffix.rend(), name.rbegin());
}

void collect_matches(const fs::path& dir,
                      const std::string& suffix,
                      std::vector<fs::path>& out) {
    if (!fs::exists(dir) || !fs::is_directory(dir))
        return;

    std::error_code ec;
    for (auto it = fs::recursive_directory_iterator(
             dir, fs::directory_options::skip_permission_denied, ec);
         it != fs::recursive_directory_iterator(); it.increment(ec)) {
        if (ec) {
            ec.clear();
            continue;
        }
        const fs::directory_entry& entry = *it;
        if (!entry.is_regular_file())
            continue;

        const std::string filename = entry.path().filename().string();
        if (has_suffix(filename, suffix))
            out.push_back(entry.path());
    }
}

} // namespace

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "usage: gen_plugins <source_root> <output_file>\n";
        return 1;
    }

    const fs::path source_root = fs::path(argv[1]);
    const fs::path output_file = fs::path(argv[2]);

    std::vector<fs::path> matches;
    collect_matches(source_root / "scene2", "_plugin.h", matches);
    collect_matches(source_root / "tools" / "reflector" / "plugins", ".h", matches);

    std::sort(matches.begin(), matches.end());
    matches.erase(std::unique(matches.begin(), matches.end()), matches.end());

    std::ostringstream out;
    out << "// Auto-generated -- do not edit.\n";
    out << "// Regenerated on every build; add a new plugin file and it\n";
    out << "// will be picked up automatically, no manual #include needed.\n";
    out << "\n";

    for (const fs::path& match : matches) {
        std::error_code ec;
        fs::path rel = fs::relative(match, source_root, ec);
        if (ec) {
            // Fall back to the absolute path if relative() fails for
            // some reason (e.g. different drives on Windows).
            rel = match;
        }
        // generic_string() always uses '/' separators, so the
        // resulting #include works on every platform/compiler.
        out << "#include \"" << rel.generic_string() << "\"\n";
    }

    const std::string content = out.str();

    // Avoid rewriting the file (and busting downstream compile caches)
    // if nothing actually changed.
    {
        std::ifstream existing(output_file, std::ios::binary);
        if (existing) {
            std::ostringstream existing_ss;
            existing_ss << existing.rdbuf();
            if (existing_ss.str() == content)
                return 0;
        }
    }

    std::error_code ec;
    fs::create_directories(output_file.parent_path(), ec);

    std::ofstream out_file(output_file, std::ios::binary | std::ios::trunc);
    if (!out_file) {
        std::cerr << "gen_plugins: failed to open '" << output_file.string()
                   << "' for writing\n";
        return 1;
    }
    out_file << content;
    return 0;
}
