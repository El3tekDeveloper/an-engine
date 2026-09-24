#pragma once
#include "resources/mesh.h"
#include "resources/material.h"
#include "scene/behaviour.h"
#include "transform_component.h"
#include "game_object.h"

class [[export]] MeshRenderer : public Behaviour {
public:
    explicit MeshRenderer(Mesh mesh, const std::vector<Material*> materials = {})
        : mesh(mesh), materials(materials) {}
    MeshRenderer() = default;
    
    [[export]] Mesh mesh;
    [[export]] std::vector<Material*> materials = {};
    
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

    void draw(RenderData& render_data) override {
        render_data.mesh_instances.push_back(MeshInstance{mesh, game_object().transform.get_matrix(), materials});
    }
};

