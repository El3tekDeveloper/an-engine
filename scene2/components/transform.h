#pragma once
#include "core/math/matrix4.h"
#include "core/math/quaternion.h"
#include "core/math/vector3.h"
#include "scene2/component.h"
#include "scene2/scene.h"
#include "scene2/registry.h"

struct [[export]] Transform : public Component {
    [[export]] Vector3 position = Vector3::Zero;
    [[export]] Quaternion rotation = Quaternion::Identity;
    [[export]] Vector3 scale = Vector3::One;
    
    static Transform matrix_to_transform(const Matrix4& matrix);
    static Matrix4 transform_to_matrix(const Transform& transform);
    
    Matrix4 get_local_matrix() const {
        return transform_to_matrix(*this);
    }
    
    Matrix4 get_world_matrix() const {
        Transform world = get_global_transform();
        return transform_to_matrix(world);
    }
    
    Transform get_local_transform() const {
        return *this;
    }
    
    Transform get_global_transform() const {
        Entity parent = owner_scene->get_parent(owner);
        if (!entity::is_valid(parent) || !owner_scene->get_registry().has<Transform>(parent))
            return *this;
        Transform parent_transform = owner_scene->get_component<Transform>(parent).get_global_transform();
        Transform result;
        Matrix4 world = transform_to_matrix(parent_transform) * transform_to_matrix(*this);
        return matrix_to_transform(world);
    }
};

inline Transform Transform::matrix_to_transform(const Matrix4& matrix) {
    Transform transform;
    transform.position = matrix.get_translation();
    transform.scale = matrix.get_scale();
    transform.rotation = Quaternion::from_matrix(matrix);
    return transform;
}

inline Matrix4 Transform::transform_to_matrix(const Transform& transform) {
    Matrix4 T = Matrix4::translation(transform.position);
    Matrix4 R = transform.rotation.normalized().to_matrix();
    Matrix4 S = Matrix4::scale(transform.scale);
    return T * R * S;
}
