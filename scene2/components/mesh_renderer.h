#pragma once
#include "component.h"
#include "resources/mesh.h"

struct [[export]] MeshRenderer : public Component {
    MeshRenderer() = default;
    explicit MeshRenderer(Mesh& mesh, const std::vector<Material*> materials = {})
        : mesh(&mesh), materials(materials) {}

    [[export]] Mesh* mesh = &Mesh::Cube;
    [[export]] std::vector<Material*> materials = {};
    
    [[export]] bool is_static = false;
    
    [[export]]
    void set_material(int index, Material* mat) {
        if (index >= 0 && index < static_cast<int>(materials.size()))
            materials[index] = mat;
    }

    [[export]]
    Material* get_material(int index) {
        if (index >= 0 && index < static_cast<int>(materials.size()))
            return materials[index];
        return nullptr;
    }

    [[export]]
    void add_material(Material* mat) {
        materials.push_back(mat);
    }

private:
    mutable Matrix4 cached_matrix = Matrix4::translation(Vector3::Zero);
    mutable bool cache_valid = false;

    friend class RenderSystem;
};

#if __REFLECT_GENERATED__
#include ".generated/mesh_renderer.generated.hxx"
#endif
