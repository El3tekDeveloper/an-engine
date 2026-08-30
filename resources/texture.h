#pragma once
#include "resources/image.h"
#include "resources/resource.h"
#include <cstdint>

enum class TextureWrap {
    Repeat,
    MirroredRepeat,
    ClampEdge,
    ClampBorder,
};

enum class TextureFilter {
    Linear,
    Nearest
};

class Texture : public Resource {
public:
    Texture() = default;
    virtual ~Texture() = default;

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&&) noexcept = default;
    Texture& operator=(Texture&&) noexcept = default;

    virtual bool load(const std::string& file_path) { return false; };
    virtual void bind(uint32_t slot = 0) const {};
    virtual void destroy() {};
    virtual bool is_valid() const { return false; };
    virtual void set_wrap(TextureWrap texture_warp) {};
    virtual void set_filter(TextureFilter texuter_filter) {};

    int get_width() const { return image.get_width(); }
    int get_height() const { return image.get_height(); }

protected:
    Image image;
    
    TextureWrap wrap = TextureWrap::ClampEdge;
    TextureFilter filter = TextureFilter::Linear;
};
