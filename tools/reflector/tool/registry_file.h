#pragma once
#include <llvm/ADT/SmallString.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/raw_ostream.h>
#include <cstring>
#include <map>
#include <string>

namespace registry_file {

constexpr const char* file_name = "project.sln.hxx";
constexpr const char* begin_mark    = "// @@BEGIN ";
constexpr const char* end_mark      = "// @@END";

inline std::string output_path() {
    llvm::SmallString<256> p;
    p = REFLECT_OUTPUT;
    llvm::sys::path::append(p, file_name);
    return p.str().str();
}

inline std::string read_all(const std::string& path) {
    auto buf = llvm::MemoryBuffer::getFile(path);
    return buf ? (*buf)->getBuffer().str() : std::string();
}

inline std::map<std::string, std::string> parse(llvm::StringRef text) {
    std::map<std::string, std::string> blocks;
    std::string key, body;
    bool in_block = false;
    while (!text.empty()) {
        auto [line, rest] = text.split('\n');
        text = rest;
        if (!in_block && line.starts_with(begin_mark)) {
            key = line.substr(std::strlen(begin_mark)).str();
            body.clear();
            in_block = true;
        } else if (in_block && line.starts_with(end_mark)) {
            blocks[key] = body;
            in_block = false;
        } else if (in_block) {
            body += line.str();
            body += '\n';
        }
    }
    return blocks;
}

constexpr const char* decl_prefix = "0/";
constexpr const char* body_prefix = "1/";

inline std::string render(const std::map<std::string, std::string>& blocks) {
    std::string out;
    out += "// Generated reflection registry\n";
    out += "// DO NOT EDIT\n";
    out += "#pragma once\n";
    out += "#include \"tools/reflector/runtime/type_registry.h\"\n\n";
    for (const auto& [key, body] : blocks) {
        out += begin_mark;
        out += key;
        out += "\n";
        out += body;
        out += end_mark;
        out += "\n\n";
    }
    return out;
}

inline void update(const std::map<std::string, std::string>& updates) {
    if (updates.empty()) return;

    const std::string path = output_path();

    llvm::sys::fs::file_t lock_fd;
    std::error_code ec = llvm::sys::fs::openFileForReadWrite(
        path + ".lock", lock_fd, llvm::sys::fs::CD_OpenAlways, llvm::sys::fs::OF_None);
    if (ec) {
        llvm::errs() << "reflect: cannot open lock file: " << ec.message() << "\n";
        return;
    }
    llvm::sys::fs::lockFile(lock_fd);

    const std::string existing = read_all(path);
    auto blocks = parse(existing);
    for (const auto& [key, body] : updates)
        blocks[key] = body;

    const std::string merged = render(blocks);

    if (merged != existing) {
        const std::string tmp = path + ".tmp";
        {
            llvm::raw_fd_ostream out(tmp, ec);
            if (ec)
                llvm::errs() << "reflect: cannot open '" << tmp << "': " << ec.message() << "\n";
            else
                out << merged;
        }
        if (!ec)
            llvm::sys::fs::rename(tmp, path);
    }

    llvm::sys::fs::unlockFile(lock_fd);
    llvm::sys::fs::closeFile(lock_fd);
}

} // namespace registry_file

