#pragma once
#include "core/os/window.h"

class RenderTarget {
public:
    virtual ~RenderTarget() = default;

    virtual void bind() = 0;
    virtual void resolve() = 0;
    virtual void resize(int width, int height) = 0;

    virtual unsigned int get_color_texture() const = 0;
    virtual int get_width() const = 0;
    virtual int get_height() const = 0;
};

class SwapchainRenderTarget : public RenderTarget {
public:
    explicit SwapchainRenderTarget(Window* window) : window(window) {}
 
    void bind() override;
    void resolve() override {}
    void resize(int, int) override {}
 
    unsigned int get_color_texture() const override { return 0; }
    int get_width() const override { return window->get_width(); }
    int get_height() const override { return window->get_height(); }
 
private:
    Window* window;
};
 
class TextureRenderTarget : public RenderTarget {
public:
    ~TextureRenderTarget() override { destroy(); }
 
    bool init(int width, int height);
    void destroy();
 
    void bind() override;
    void resolve() override;
    void resize(int width, int height) override;
 
    unsigned int get_color_texture() const override { return resolve_color_tex_id; }
    int get_width() const override { return m_width; }
    int get_height() const override { return m_height; }
 
private:
    unsigned int fbo_id = 0;
    unsigned int fbo_color_rbo_id = 0;
    unsigned int fbo_depth_rbo_id = 0;
 
    unsigned int resolve_fbo_id = 0;
    unsigned int resolve_color_tex_id = 0;
 
    int m_width = 0;
    int m_height = 0;
};

