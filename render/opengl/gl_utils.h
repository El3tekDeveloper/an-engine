#pragma once
#include "core/debug/logger.h"
#include "core/io/file_access.h"
#include "core/io/path.h"
#include "core/memory/bump_allocator.h"
#include "render/renderer.h"
#include "resources/gpu_types.h"
#include "resources/material.h"
#include <string>
#include <glad/glad.h>

static const std::string shaders_folder = "engine://render/shaders/";
static const std::string shared_folder = "engine://resources/";

void inline APIENTRY gl_debug_callback([[maybe_unused]] GLenum source, [[maybe_unused]] GLenum type, [[maybe_unused]] GLuint id, GLenum severity,
                                [[maybe_unused]] GLsizei length, const GLchar* message, [[maybe_unused]] const void* user) {
#if ENGINE_DEBUG_ENABLED
    if( severity == GL_DEBUG_SEVERITY_LOW || 
        severity == GL_DEBUG_SEVERITY_MEDIUM ||
        severity == GL_DEBUG_SEVERITY_HIGH) {
        LOG_ERROR("OpenGL: {}", message);
    }
    else {
        LOG_INFO("{}", (const char*)message);
    }
#else
    (void)severity; (void)message;
#endif
}

static inline GLuint create_shader(ShaderType type, const std::string& shader_file, const std::vector<GpuHandle>& headers, BumpAllocator& allocator) {
    GLenum shader_type = type == ShaderType::Vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;

    const std::string shader_src = IO::read_file(IO::join(shaders_folder, shader_file), allocator);
    
    std::string shader_header;
    for (auto& header : headers) {
        shader_header += header.get_string();
    }

    const std::string src = 
        "#version 430 core     \n"
        + shader_header + "\n"
        + shader_src;
    const char* src_ptr = src.c_str();
    
    GLuint shader_id = glCreateShader(shader_type);
    glShaderSource(shader_id, 1, &src_ptr, nullptr);
    glCompileShader(shader_id);

    GLint success;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[1024];
        glGetShaderInfoLog(shader_id, sizeof(log), nullptr, log);
        LOG_ERROR("Shader compile error ({}):\n{}", shader_file.c_str(), log);
        glDeleteShader(shader_id);
        return 0;
    }

    return shader_id;
}

static inline GLuint create_program(const std::string& vert_file, const std::string& frag_file, const std::vector<GpuHandle>& headers, BumpAllocator& allocator) {
    GLuint vert = create_shader(ShaderType::Vertex, vert_file, headers, allocator);
    GLuint frag = create_shader(ShaderType::Fragment, frag_file, headers, allocator);
    if (!vert || !frag) {
        glDeleteShader(vert);
        glDeleteShader(frag);
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    glDetachShader(program, vert);
    glDetachShader(program, frag);
    glDeleteShader(vert);
    glDeleteShader(frag);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[1024];
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        LOG_ERROR("Program link error ({} + {}):\n{}",
               vert_file.c_str(), frag_file.c_str(), log);
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

static inline void bind_material(uint program, Material* material) {
    auto bind_slot = [&](const char* name, int slot, Texture* texture) {
        glUniform1i(glGetUniformLocation(program, name), slot);
        if (texture && texture->is_valid()) {
            texture->bind(slot);
        } else {
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    };

    bind_slot("texture_albedo",     MATERIAL_SLOT_ALBEDO,     material->albedo_texture);
    bind_slot("texture_normal",     MATERIAL_SLOT_NORMAL,     material->normal_texture);
    bind_slot("texture_metalrough", MATERIAL_SLOT_METALROUGH, material->metalrough_texture);
    bind_slot("texture_emissive",   MATERIAL_SLOT_EMISSIVE,   material->emissive_texture);
}
