#include "render/renderer.h"
#include "gl_utils.h"
#include "core/math/matrix4.h"
#include "resources/opengl/gl_texture.h"
#include "resources/resource_manager.h"
#include "resources/mesh.h"
#include "resources/vertex.h"
#include <glad/glad.h>
#include <string>
#include <stb_image.h>

static constexpr GLsizei FBO_MSAA_SAMPLES = 4;

struct GLContext {
    GLuint vao_id;
    
    GLuint mesh_vbo_id;
    GLuint mesh_ibo_id;
    GLuint mesh_program_id;
    GLuint material_ubo_id;
    Material default_material;

    GLint camera_loc;
    GLint model_loc;
    GLint index_offset_loc;

    GLuint fbo_id = 0;
    GLuint fbo_color_rbo_id = 0;
    GLuint fbo_depth_rbo_id = 0;

    GLuint resolve_fbo_id = 0;
    GLuint resolve_color_tex_id = 0;

    int fbo_width = 0;
    int fbo_height = 0;
};

static GLContext context;

static bool create_framebuffers(int width, int height) {
    if (width <= 0 || height <= 0)
        return false;

    if (width == context.fbo_width && height == context.fbo_height && context.fbo_id != 0)
        return true;

    if (context.fbo_id) {
        glDeleteFramebuffers(1, &context.fbo_id);
        context.fbo_id = 0;
    }
    if (context.fbo_color_rbo_id) {
        glDeleteRenderbuffers(1, &context.fbo_color_rbo_id);
        context.fbo_color_rbo_id = 0;
    }
    if (context.fbo_depth_rbo_id) {
        glDeleteRenderbuffers(1, &context.fbo_depth_rbo_id);
        context.fbo_depth_rbo_id = 0;
    }
    if (context.resolve_fbo_id) {
        glDeleteFramebuffers(1, &context.resolve_fbo_id);
        context.resolve_fbo_id = 0;
    }
    if (context.resolve_color_tex_id) {
        glDeleteTextures(1, &context.resolve_color_tex_id);
        context.resolve_color_tex_id = 0;
    }

    glGenFramebuffers(1, &context.fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, context.fbo_id);

    glGenRenderbuffers(1, &context.fbo_color_rbo_id);
    glBindRenderbuffer(GL_RENDERBUFFER, context.fbo_color_rbo_id);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, FBO_MSAA_SAMPLES, GL_SRGB8_ALPHA8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, context.fbo_color_rbo_id);

    glGenRenderbuffers(1, &context.fbo_depth_rbo_id);
    glBindRenderbuffer(GL_RENDERBUFFER, context.fbo_depth_rbo_id);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, FBO_MSAA_SAMPLES, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, context.fbo_depth_rbo_id);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }

    glGenFramebuffers(1, &context.resolve_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, context.resolve_fbo_id);

    glGenTextures(1, &context.resolve_color_tex_id);
    glBindTexture(GL_TEXTURE_2D, context.resolve_color_tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, context.resolve_color_tex_id, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    context.fbo_width = width;
    context.fbo_height = height;
    return true;
}

bool Renderer::init(Window* _window, BumpAllocator* transient_storage) {
    window = _window;

    ResourceManager.get_registry<Texture>().set_factory(
        []{ return std::make_unique<GLTexture>(); }
    );

#if ENGINE_DEBUG_ENABLED
    glDebugMessageCallback(&gl_debug_callback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_DEBUG_OUTPUT);
#endif

    context.mesh_program_id = create_program("mesh.vert", "mesh.frag", {"vertex.h", "material.h"}, *transient_storage);
    if (!context.mesh_program_id) return false;
    glUseProgram(context.mesh_program_id);

    glGenBuffers(1, &context.mesh_vbo_id);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, context.mesh_vbo_id);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &context.mesh_ibo_id);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, context.mesh_ibo_id);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &context.material_ubo_id);
    glBindBuffer(GL_UNIFORM_BUFFER, context.material_ubo_id);
    glBufferData(GL_UNIFORM_BUFFER, Material::gpu_size(), nullptr, GL_DYNAMIC_DRAW);
    glUniform1i(glGetUniformLocation(context.mesh_program_id, "texture_albedo"), MATERIAL_SLOT_ALBEDO);
    glUniform1i(glGetUniformLocation(context.mesh_program_id, "texture_normal"), MATERIAL_SLOT_NORMAL);
    glUniform1i(glGetUniformLocation(context.mesh_program_id, "texture_metalrough"), MATERIAL_SLOT_METALROUGH);
    glUniform1i(glGetUniformLocation(context.mesh_program_id, "texture_emissive"), MATERIAL_SLOT_EMISSIVE);

    context.camera_loc = glGetUniformLocation(context.mesh_program_id, "camera");
    context.model_loc = glGetUniformLocation(context.mesh_program_id, "model");
    context.index_offset_loc = glGetUniformLocation(context.mesh_program_id, "index_offset");

    glGenVertexArrays(1, &context.vao_id);
    glBindVertexArray(context.vao_id);

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_MULTISAMPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (!create_framebuffers(window->width, window->height)) {
        return false;
    }

    return true;
}

void Renderer::render(RenderData& render_data) {
    if (!is_valid())
        return;

    viewport_ptr = &render_data.camera->viewport;
    create_framebuffers(viewport_ptr->width, viewport_ptr->height);

    glBindFramebuffer(GL_FRAMEBUFFER, context.fbo_id);
    glViewport(viewport_ptr->position.x, viewport_ptr->position.y,
            viewport_ptr->width, viewport_ptr->height);

    glClearColor(
        render_data.clear_color.r,
        render_data.clear_color.g,
        render_data.clear_color.b,
        render_data.clear_color.a
    );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(context.mesh_program_id);

    glUniformMatrix4fv(context.camera_loc, 1, GL_TRUE, render_data.camera->matrix.data());

    const Mesh* last_uploaded_mesh = nullptr;

    for (MeshInstance& instance : render_data.mesh_instances) {
        if (instance.mesh != last_uploaded_mesh) {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, context.mesh_vbo_id);
            glBufferData(GL_SHADER_STORAGE_BUFFER,
                instance.mesh->vertices.size() * sizeof(Vertex),
                nullptr, GL_STREAM_DRAW);
            glBufferData(GL_SHADER_STORAGE_BUFFER,
                instance.mesh->vertices.size() * sizeof(Vertex),
                instance.mesh->vertices.data(), GL_STREAM_DRAW);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, context.mesh_vbo_id);

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, context.mesh_ibo_id);
            glBufferData(GL_SHADER_STORAGE_BUFFER,
                instance.mesh->indices.size() * sizeof(uint32_t),
                nullptr, GL_STREAM_DRAW);
            glBufferData(GL_SHADER_STORAGE_BUFFER,
                instance.mesh->indices.size() * sizeof(uint32_t),
                instance.mesh->indices.data(), GL_STREAM_DRAW);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, context.mesh_ibo_id);

            last_uploaded_mesh = instance.mesh;
        }

        glUniformMatrix4fv(
            context.model_loc,
            1, GL_TRUE, instance.model.data()
        );

        for (const SubMesh& sub : instance.mesh->submeshes) {
            Material* mat_base = (sub.material_index < instance.materials.size())
                ? instance.materials[sub.material_index]
                : nullptr;

            Material* mat = mat_base
                ? static_cast<Material*>(mat_base)
                : &context.default_material;

            const char* gpu_start = reinterpret_cast<const char*>(mat) + offsetof(Material, albedo_factor);
            glBindBuffer(GL_UNIFORM_BUFFER, context.material_ubo_id);
            glBufferData(GL_UNIFORM_BUFFER, Material::gpu_size(), nullptr, GL_STREAM_DRAW);
            glBufferData(GL_UNIFORM_BUFFER, Material::gpu_size(), gpu_start, GL_STREAM_DRAW);
            glBindBufferBase(GL_UNIFORM_BUFFER, 2, context.material_ubo_id);

            bind_material(context.mesh_program_id, mat);

            glUniform1ui(context.index_offset_loc, sub.index_offset);
            glDrawArrays(GL_TRIANGLES, 0, sub.index_count);
        }

    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, context.fbo_id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, context.resolve_fbo_id);
    glBlitFramebuffer(
        0, 0, viewport_ptr->width, viewport_ptr->height,
        0, 0, viewport_ptr->width, viewport_ptr->height,
        GL_COLOR_BUFFER_BIT, GL_NEAREST
    );

    glBindFramebuffer(GL_READ_FRAMEBUFFER, context.resolve_fbo_id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(
        0, 0, viewport_ptr->width, viewport_ptr->height,
        viewport_ptr->position.x, viewport_ptr->position.y,
        viewport_ptr->position.x + viewport_ptr->width,
        viewport_ptr->position.y + viewport_ptr->height,
        GL_COLOR_BUFFER_BIT, GL_NEAREST
    );

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    render_data.mesh_instances.clear();
}

void Renderer::resize(int width, int height) {
    viewport_ptr->resize(width, height);
    glViewport(0, 0, width, height);
    create_framebuffers(width, height);
}

uint Renderer::get_output_texture() const {
    return context.resolve_color_tex_id;
}

void Renderer::destroy() {
    if (!window) return;

    glDeleteBuffers(1, &context.material_ubo_id);
    glDeleteBuffers(1, &context.mesh_vbo_id);
    glDeleteBuffers(1, &context.mesh_ibo_id);
    glDeleteVertexArrays(1, &context.vao_id);
    glDeleteProgram(context.mesh_program_id);

    if (context.fbo_id) glDeleteFramebuffers(1, &context.fbo_id);
    if (context.resolve_fbo_id) glDeleteFramebuffers(1, &context.resolve_fbo_id);
    if (context.fbo_color_rbo_id) glDeleteRenderbuffers(1, &context.fbo_color_rbo_id);
    if (context.fbo_depth_rbo_id) glDeleteRenderbuffers(1, &context.fbo_depth_rbo_id);
    if (context.resolve_color_tex_id) glDeleteTextures(1, &context.resolve_color_tex_id);

    window = nullptr;
}
