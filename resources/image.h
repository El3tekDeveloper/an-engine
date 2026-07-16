#pragma once
#include "core/io/path.h"
#include "stb_image.h"
#include <vector>
#include <string>

class Image{
public:
    Image() = default;
    ~Image() = default;

    bool load_from_file(const std::string& path, bool flip = true) {
        int channels;
        stbi_set_flip_vertically_on_load(flip);
        unsigned char* data = stbi_load(IO::resolve_path(path).c_str(), &width, &height, &channels, 4);
        if (!data) return false;

        pixels.assign(data, data + width * height * 4);
        stbi_image_free(data);
        return true;
    }

    int get_width() const { return width; }
    int get_height() const { return height; }
    
    void resize(int w, int h) {
        width = w;
        height = h;
        pixels.resize(width * height * 4);
    }

    size_t size() const { return pixels.size(); }
    const unsigned char* data() const { return pixels.data(); }

private:
    int width = 0;
    int height = 0;
    std::vector<unsigned char> pixels;
};
