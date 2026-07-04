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

struct GLContext {
    GLuint vao_id;
    GLuint mesh_vbo_id;
    GLuint mesh_ibo_id;
    GLuint mesh_program_id;
    GLuint material_ubo_id;
    Material default_material;
};

static GLContext context;

bool Renderer::init(Window* _window, BumpAllocator* transient_storage) {
    window = _window;
    
    ResourceManager.set_impls(
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
    
    glGenVertexArrays(1, &context.vao_id);
    glBindVertexArray(context.vao_id);

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_MULTISAMPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    return true;
}

void Renderer::render(RenderData& render_data) {
    if (!is_valid())
        return;
 
    ViewPort& vp = render_data.camera->viewport;
    viewport_ptr = &render_data.camera->viewport;
    glViewport(0, 0, vp.width, vp.height);
    
    glClearColor(
        render_data.clear_color.r,
        render_data.clear_color.g,
        render_data.clear_color.b,
        render_data.clear_color.a
    );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(context.mesh_program_id);
    
    // context.default_material.bind(context.mesh_program_id);
    glUniformMatrix4fv(glGetUniformLocation(context.mesh_program_id, "camera"), 1, GL_TRUE, render_data.camera->matrix.data());

    for (MeshInstance& instance : render_data.mesh_instances) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, context.mesh_vbo_id);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
            instance.mesh.vertices.size() * sizeof(Vertex),
            instance.mesh.vertices.data(), GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, context.mesh_vbo_id);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, context.mesh_ibo_id);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
            instance.mesh.indices.size() * sizeof(uint32_t),
            instance.mesh.indices.data(), GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, context.mesh_ibo_id);

        glUniformMatrix4fv(
            glGetUniformLocation(context.mesh_program_id, "model"),
            1, GL_TRUE, instance.model.data()
        );

        for (const SubMesh& sub : instance.mesh.submeshes) {
            Material* mat_base = (sub.material_index < instance.materials.size())
                ? instance.materials[sub.material_index]
                : nullptr;

            Material* mat = mat_base
                ? static_cast<Material*>(mat_base)
                : &context.default_material;
            
            const char* gpu_start = reinterpret_cast<const char*>(mat) + offsetof(Material, albedo_factor);
            glBindBuffer(GL_UNIFORM_BUFFER, context.material_ubo_id);
            glBufferData(GL_UNIFORM_BUFFER, Material::gpu_size(), gpu_start, GL_DYNAMIC_DRAW);
            glBindBufferBase(GL_UNIFORM_BUFFER, 2, context.material_ubo_id);

            bind_material(context.mesh_program_id, mat);

            glUniform1ui(glGetUniformLocation(context.mesh_program_id, "index_offset"), sub.index_offset);
            glDrawArrays(GL_TRIANGLES, 0, sub.index_count);
        }

    }

    render_data.mesh_instances.clear();
}

void Renderer::resize(int width, int height) {
    viewport_ptr->resize(width, height);
    glViewport(0, 0, width, height);
}

void Renderer::destroy() {
    if (!window) return;

    glDeleteBuffers(1, &context.material_ubo_id);
    glDeleteBuffers(1, &context.mesh_vbo_id);
    glDeleteBuffers(1, &context.mesh_ibo_id);
    glDeleteVertexArrays(1, &context.vao_id);
    glDeleteProgram(context.mesh_program_id);

    window = nullptr;
}
