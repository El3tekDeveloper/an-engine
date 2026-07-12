#pragma once
#include "core/math/matrix4.h"
#include "resources/material.h"
#include "resources/vertex.h"
#include <cstdint>
#include <vector>

struct SubMesh {
    uint32_t index_offset = 0;
    uint32_t index_count = 0;
    uint32_t material_index = 0;
};

struct Mesh : Resource {
public:
    static const Mesh Cube;
    static const Mesh Sphere;
    static const Mesh Capsule;

    explicit Mesh(std::string name, std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<SubMesh> submeshes = {})
        : name(name), vertices(vertices), indices(indices), submeshes(submeshes) {
        if (this->submeshes.empty()) {
            this->submeshes.push_back({
                .index_offset = 0,
                .index_count = (uint32_t)this->indices.size(),
                .material_index = 0
            });
        }

        bounding_radius = 0.0f;
        for (const Vertex& vertex : this->vertices) {
            bounding_radius = std::max(bounding_radius, vertex.position.length());
        }
    }

    Mesh() = default;
    ~Mesh() {};
    
    std::string name;

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<SubMesh> submeshes;

    float bounding_radius = 0.0f;
};

struct MeshInstance {
    const Mesh* mesh = &Mesh::Cube;
    Matrix4 model = Matrix4::Identity;
    std::vector<Material*> materials = {};
};

inline const Mesh Mesh::Cube = Mesh(
    "Cube",
    {
        // Front  (normal  0, 0, 1)
        {{ -0.5f, -0.5f,  0.5f }, { 0,0,1 }, { 0,0 }},
        {{  0.5f, -0.5f,  0.5f }, { 0,0,1 }, { 1,0 }},
        {{  0.5f,  0.5f,  0.5f }, { 0,0,1 }, { 1,1 }},
        {{ -0.5f,  0.5f,  0.5f }, { 0,0,1 }, { 0,1 }},
        // Back   (normal  0, 0,-1)
        {{  0.5f, -0.5f, -0.5f }, { 0,0,-1 }, { 0,0 }},
        {{ -0.5f, -0.5f, -0.5f }, { 0,0,-1 }, { 1,0 }},
        {{ -0.5f,  0.5f, -0.5f }, { 0,0,-1 }, { 1,1 }},
        {{  0.5f,  0.5f, -0.5f }, { 0,0,-1 }, { 0,1 }},
        // Left   (normal -1, 0, 0)
        {{ -0.5f, -0.5f, -0.5f }, { -1,0,0 }, { 0,0 }},
        {{ -0.5f, -0.5f,  0.5f }, { -1,0,0 }, { 1,0 }},
        {{ -0.5f,  0.5f,  0.5f }, { -1,0,0 }, { 1,1 }},
        {{ -0.5f,  0.5f, -0.5f }, { -1,0,0 }, { 0,1 }},
        // Right  (normal  1, 0, 0)
        {{  0.5f, -0.5f,  0.5f }, { 1,0,0 }, { 0,0 }},
        {{  0.5f, -0.5f, -0.5f }, { 1,0,0 }, { 1,0 }},
        {{  0.5f,  0.5f, -0.5f }, { 1,0,0 }, { 1,1 }},
        {{  0.5f,  0.5f,  0.5f }, { 1,0,0 }, { 0,1 }},
        // Top    (normal  0, 1, 0)
        {{ -0.5f,  0.5f,  0.5f }, { 0,1,0 }, { 0,0 }},
        {{  0.5f,  0.5f,  0.5f }, { 0,1,0 }, { 1,0 }},
        {{  0.5f,  0.5f, -0.5f }, { 0,1,0 }, { 1,1 }},
        {{ -0.5f,  0.5f, -0.5f }, { 0,1,0 }, { 0,1 }},
        // Bottom (normal  0,-1, 0)
        {{ -0.5f, -0.5f, -0.5f }, { 0,-1,0 }, { 0,0 }},
        {{  0.5f, -0.5f, -0.5f }, { 0,-1,0 }, { 1,0 }},
        {{  0.5f, -0.5f,  0.5f }, { 0,-1,0 }, { 1,1 }},
        {{ -0.5f, -0.5f,  0.5f }, { 0,-1,0 }, { 0,1 }},
    },
    {
         0,  1,  2,   0,  2,  3,
         4,  5,  6,   4,  6,  7,
         8,  9, 10,   8, 10, 11,
        12, 13, 14,  12, 14, 15,
        16, 17, 18,  16, 18, 19,
        20, 21, 22,  20, 22, 23,
    }
);

inline const Mesh Mesh::Sphere = []() {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    const float radius = 0.5f;
    const int stacks = 32;   // latitude
    const int slices = 32;   // longitude

    for (int i = 0; i <= stacks; i++) {
        float v = (float)i / stacks;
        float phi = v * M_PI; // 0 → PI

        for (int j = 0; j <= slices; j++) {
            float u = (float)j / slices;
            float theta = u * M_PI * 2.0f; // 0 → 2PI

            float x = sin(phi) * cos(theta);
            float y = cos(phi);
            float z = sin(phi) * sin(theta);

            Vector3 pos = { x * radius, y * radius, z * radius };
            Vector3 normal = Vector3(x, y, z).normalized();
            Vector2 uv = { u, v };

            vertices.push_back({ pos, normal, uv });
        }
    }

    for (int i = 0; i < stacks; i++) {
        for (int j = 0; j < slices; j++) {

            int first = i * (slices + 1) + j;
            int second = first + slices + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    return Mesh("Sphere", vertices, indices);
}();

inline const Mesh Mesh::Capsule = []() {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    const float radius     = 0.5f;
    const float cylHeight  = 1.0f;
    const int   slices     = 32;
    const int   hemiStacks = 8;

    const float halfH = cylHeight * 0.5f;
    const int   totalRings = 2 * hemiStacks + 2;

    for (int i = 0; i < totalRings; i++) {
        float phi;
        float yOff;

        if (i <= hemiStacks) {
            phi  = (static_cast<float>(i) / hemiStacks) * (float)(M_PI * 0.5);
            yOff = halfH;
        } else {
            int j = i - (hemiStacks + 1);
            phi  = (float)(M_PI * 0.5) + (static_cast<float>(j) / hemiStacks) * (float)(M_PI * 0.5);
            yOff = -halfH;
        }

        float v = static_cast<float>(i) / (totalRings - 1);

        for (int j = 0; j <= slices; j++) {
            float u = static_cast<float>(j) / slices;
            float theta = u * (float)(M_PI * 2.0);

            float sx = sin(phi) * cos(theta);
            float sy = cos(phi);
            float sz = sin(phi) * sin(theta);

            Vector3 pos    = { sx * radius, sy * radius + yOff, sz * radius };
            Vector3 normal = Vector3(sx, sy, sz).normalized();
            Vector2 uv     = { u, v };

            vertices.push_back({ pos, normal, uv });
        }
    }

    for (int i = 0; i < totalRings - 1; i++) {
        for (int j = 0; j < slices; j++) {
            int first  = i * (slices + 1) + j;
            int second = first + slices + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    return Mesh("Capsule", vertices, indices);
}();
