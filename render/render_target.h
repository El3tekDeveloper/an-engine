#pragma once
#include "core/os/window.h"

class RenderTarget {
public:
    virtual ~RenderTarget() = default;

    virtual void bind() {};
    virtual void resolve() {};
    virtual void resize(int width, int height) {};

    virtual unsigned int get_color_texture() const = 0;
    virtual int get_width() const = 0;
    virtual int get_height() const = 0;
};

class SwapchainRenderTarget : public RenderTarget {
public:
    explicit SwapchainRenderTarget(Window* window) : window(window) {}
 
    void bind() override;
    
    unsigned int get_color_texture() const override { return 0; }
    int get_width() const override { return window->get_width(); }
    int get_height() const override { return window->get_height(); }
 
private:
    Window* window;
};
 
class TextureRenderTarget : public RenderTarget {
public:
    ~TextureRenderTarget() override { destroy(); }
 
    bool init(int x, int y, int width, int height);
    void destroy();

    inline void set_position(int x, int y) { m_x = x; m_y = y; }
 
    void bind() override;
    void resolve() override;
    void resize(int width, int height) override;
 
    unsigned int get_color_texture() const override { return resolve_color_tex_id; }

    int get_width() const override { return m_width; }
    int get_height() const override { return m_height; }

    int get_x() const { return m_x; }
    int get_y() const { return m_y; }

    unsigned int get_resolve_framebuffer() const { return resolve_fbo_id; }
 
private:
    unsigned int fbo_id = 0;
    unsigned int fbo_color_rbo_id = 0;
    unsigned int fbo_depth_rbo_id = 0;
 
    unsigned int resolve_fbo_id = 0;
    unsigned int resolve_color_tex_id = 0;
 
    int m_width = 0;
    int m_height = 0;

    int m_x, m_y;

};

