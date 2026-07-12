#pragma once
#include <algorithm>
#include <filesystem>
#include <glad/glad.h>
#include "core/utils/uuid.h"
#include "resources/texture.h"
#include "core/debug/logger.h"

class GLTexture : public Texture {
public:
    GLTexture() = default;
    ~GLTexture() override { destroy(); }

    GLTexture(const GLTexture&) = delete;
    GLTexture& operator=(const GLTexture&) = delete;

    GLTexture(GLTexture&& other) noexcept
        : Texture(std::move(other)) {}

    GLTexture& operator=(GLTexture&& other) noexcept {
        if (this != &other) {
            destroy();
            Texture::operator=(std::move(other));
            wrap = other.wrap;
            filter = other.filter;
        }
        return *this;
    }

    bool load(const std::string& file_path) override {
        LOG_INFO("CWD: {}", std::filesystem::current_path().string());
        LOG_INFO("Exists: {}", std::filesystem::exists(file_path));
        if (!image.load_from_file(file_path, true)) {
            LOG_ERROR("Failed to load texture: '{}'", file_path.c_str());
            return false;
        }
        uuid = make_uuid(file_path);
        
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, to_gl(wrap));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, to_gl(wrap));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, to_gl(filter));

        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8,
                     image.get_width(), image.get_height(),
                     0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());

        glGenerateMipmap(GL_TEXTURE_2D);
        
        GLenum min_filter = (to_gl(filter) == GL_LINEAR)
            ? GL_LINEAR_MIPMAP_LINEAR
            : GL_NEAREST_MIPMAP_NEAREST;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);

        float max_aniso = 1.0f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &max_aniso);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, std::clamp(max_aniso, 1.0f, 8.0f));

        return true;
    }
    
    bool is_valid() const override {
        return (image.get_width() != 0 || image.get_height() != 0);
    }

    void destroy() override {
        if (id) {
            glDeleteTextures(1, &id);
            id = 0;
        }
    }

    void bind(uint32_t slot = 0) const override {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, id);
    }

    uint get_id() { return id; }

    void set_wrap(TextureWrap texture_warp) override {
        wrap = texture_warp;
        if (!id) return;
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, to_gl(texture_warp));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, to_gl(texture_warp));
    }

    void set_filter(TextureFilter texuter_filter) override {
        filter = texuter_filter;
        if (!id) return;
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, to_gl(texuter_filter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, to_gl(texuter_filter));
    }

private:
    uint32_t id = 0;

    static GLint to_gl(TextureWrap w) {
        switch (w) {
            case TextureWrap::Repeat:         return GL_REPEAT;
            case TextureWrap::MirroredRepeat: return GL_MIRRORED_REPEAT;
            case TextureWrap::ClampEdge:      return GL_CLAMP_TO_EDGE;
            case TextureWrap::ClampBorder:    return GL_CLAMP_TO_BORDER;
        }
        return GL_CLAMP_TO_EDGE;
    }

    static GLint to_gl(TextureFilter f) {
        return f == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST;
    }
};
