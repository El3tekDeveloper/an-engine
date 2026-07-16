#include "render/renderer.h"
#include "gl_utils.h"
#include "core/math/matrix4.h"
#include "core/math/vector2.h"
#include "resources/material.h"
#include "resources/opengl/gl_texture.h"
#include "resources/resource_manager.h"
#include "resources/mesh.h"
#include "resources/vertex.h"
#include <glad/glad.h>
#include <algorithm>
#include <string>
#include <stb_image.h>

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

    GLuint sprite_program_id;
    GLint sprite_camera_loc;
    GLint sprite_model_loc;
    GLint sprite_uv_rect_loc;
    GLint sprite_transform_loc;
    GLint sprite_tint_loc;
};

static GLContext context;

bool Renderer::init(BumpAllocator* transient_storage) {
    ResourceManager.get_registry<Texture>().set_factory(
        []{ return std::make_unique<GLTexture>(); }
    );

#if ENGINE_DEBUG_ENABLED
    glDebugMessageCallback(&gl_debug_callback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_DEBUG_OUTPUT);
#endif

    context.mesh_program_id = create_program("mesh.vert", "mesh.frag", {Vertex::get_gpu_handle(), Material::get_gpu_handle()}, *transient_storage);
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

    context.sprite_program_id = create_program("sprite.vert", "sprite.frag", {}, *transient_storage);
    if (!context.sprite_program_id) return false;
    glUseProgram(context.sprite_program_id);
    glUniform1i(glGetUniformLocation(context.sprite_program_id, "sprite_texture"), 0);

    context.sprite_camera_loc = glGetUniformLocation(context.sprite_program_id, "camera");
    context.sprite_model_loc = glGetUniformLocation(context.sprite_program_id, "model");
    context.sprite_uv_rect_loc = glGetUniformLocation(context.sprite_program_id, "uv_rect");
    context.sprite_transform_loc = glGetUniformLocation(context.sprite_program_id, "sprite_transform");
    context.sprite_tint_loc = glGetUniformLocation(context.sprite_program_id, "tint_color");

    glGenVertexArrays(1, &context.vao_id);
    glBindVertexArray(context.vao_id);

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_MULTISAMPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    initialized = true;
    return true;
}

void Renderer::render(ViewPort& viewport, RenderData& render_data) {
    if (!initialized || !viewport.target)
        return;

    RenderTarget* target = viewport.target;
    target->resize(viewport.width, viewport.height);
    target->bind();
    glViewport(0, 0, viewport.width, viewport.height);

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

    if (!render_data.sprite_instances.empty()) {
        std::sort(render_data.sprite_instances.begin(), render_data.sprite_instances.end(),
            [](const SpriteInstance& a, const SpriteInstance& b) {
                return a.layer < b.layer;
            });

        glUseProgram(context.sprite_program_id);
        glUniformMatrix4fv(context.sprite_camera_loc, 1, GL_TRUE, render_data.camera->matrix.data());

        for (const SpriteInstance& sprite_instance : render_data.sprite_instances) {
            const Sprite* sprite = sprite_instance.sprite;
            if (!sprite || !sprite->is_valid()) continue;

            Vector2 uv_min, uv_max;
            sprite->get_uv_rect(uv_min, uv_max);

            Vector2 size = sprite->get_size();
            Vector2 offset(-sprite->pivot.x * size.x, -sprite->pivot.y * size.y);

            if (sprite_instance.flip_x) { size.x = -size.x; offset.x = -offset.x; }
            if (sprite_instance.flip_y) { size.y = -size.y; offset.y = -offset.y; }

            glUniformMatrix4fv(context.sprite_model_loc, 1, GL_TRUE, sprite_instance.model.data());
            glUniform4f(context.sprite_uv_rect_loc, uv_min.x, uv_min.y, uv_max.x, uv_max.y);
            glUniform4f(context.sprite_transform_loc, offset.x, offset.y, size.x, size.y);
            glUniform4f(context.sprite_tint_loc,
                sprite_instance.color.r, sprite_instance.color.g,
                sprite_instance.color.b, sprite_instance.color.a);

            sprite->texture->bind(0);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }

    target->resolve();

    render_data.mesh_instances.clear();
}

void Renderer::destroy() {
    if (!initialized) return;

    glDeleteBuffers(1, &context.material_ubo_id);
    glDeleteBuffers(1, &context.mesh_vbo_id);
    glDeleteBuffers(1, &context.mesh_ibo_id);
    glDeleteVertexArrays(1, &context.vao_id);
    glDeleteProgram(context.mesh_program_id);
    glDeleteProgram(context.sprite_program_id);

    initialized = false;
}
