#pragma once
#include <cmath>
#include "core/math/matrix4.h"
#include "core/math/quaternion.h"
#include "core/math/vector3.h"
#include "scene2/component.h"
#include "scene2/scene.h"
#include "scene2/registry.h"

namespace transform_detail {
    inline Vector3 mul(const Vector3& a, const Vector3& b) {
        return { a.x * b.x, a.y * b.y, a.z * b.z };
    }
    inline Vector3 div(const Vector3& a, const Vector3& b) {
        return { b.x != 0.0f ? a.x / b.x : 0.0f,
                 b.y != 0.0f ? a.y / b.y : 0.0f,
                 b.z != 0.0f ? a.z / b.z : 0.0f };
    }
    inline Vector3 any_perpendicular(const Vector3& v) {
        Vector3 helper = std::fabs(v.x) < 0.9f ? Vector3::Right : Vector3::Forward;
        return helper.cross(v).normalized();
    }
    inline Quaternion look_rotation(const Vector3& forward, const Vector3& up = Vector3::Up) {
        Vector3 z = forward.normalized();
        if (z.is_zero_approx()) return Quaternion::Identity;
        Vector3 x = up.cross(z);
        x = x.is_zero_approx() ? any_perpendicular(z) : x.normalized();
        Vector3 y = z.cross(x);

        Matrix4 m = Matrix4::Identity;
        m[0][0] = x.x; m[0][1] = y.x; m[0][2] = z.x;
        m[1][0] = x.y; m[1][1] = y.y; m[1][2] = z.y;
        m[2][0] = x.z; m[2][1] = y.z; m[2][2] = z.z;
        return Quaternion::from_matrix(m).normalized();
    }
    inline Quaternion from_to_rotation(const Vector3& from, const Vector3& to) {
        Vector3 f = from.normalized();
        Vector3 t = to.normalized();
        float d = f.dot(t);
        if (d < -0.9999f)
            return Quaternion::from_axis_angle(any_perpendicular(f), Math::PI);
        Vector3 c = f.cross(t);
        return Quaternion(c.x, c.y, c.z, 1.0f + d).normalized();
    }
}

struct [[export]] Transform : public Component {
    [[export]] Vector3 position = Vector3::Zero;
    [[export]] Quaternion rotation = Quaternion::Identity;
    [[export]] Vector3 scale = Vector3::One;

    enum class Space { Self, World };

    static Transform matrix_to_transform(const Matrix4& matrix);
    static Matrix4 transform_to_matrix(const Transform& transform);

    const Transform* get_parent_transform() const {
        Entity parent = owner_scene->get_parent(owner);
        if (!entity::is_valid(parent) || !owner_scene->get_registry().has<Transform>(parent))
            return nullptr;
        return &owner_scene->get_component<Transform>(parent);
    }

    Matrix4 get_local_matrix() const {
        return transform_to_matrix(*this);
    }

    Matrix4 get_world_matrix() const {
        const Transform* p = get_parent_transform();
        Matrix4 local = transform_to_matrix(*this);
        return p ? p->get_world_matrix() * local : local;
    }

    Transform get_local_transform() const {
        return *this;
    }

    Transform get_global_transform() const {
        Transform result = *this;
        result.position = get_world_position();
        result.rotation = get_world_rotation();
        result.scale    = get_lossy_scale();
        return result;
    }

    Vector3 get_world_position() const {
        const Transform* p = get_parent_transform();
        return p ? p->transform_point(position) : position;
    }

    void set_world_position(const Vector3& world_pos) {
        const Transform* p = get_parent_transform();
        position = p ? p->inverse_transform_point(world_pos) : world_pos;
    }

    Quaternion get_world_rotation() const {
        const Transform* p = get_parent_transform();
        Quaternion local = rotation.normalized();
        return p ? (p->get_world_rotation() * local).normalized() : local;
    }

    void set_world_rotation(const Quaternion& world_rot) {
        const Transform* p = get_parent_transform();
        Quaternion q = world_rot.normalized();
        rotation = p ? (p->get_world_rotation().conjugate() * q).normalized() : q;
    }

    Vector3 get_lossy_scale() const {
        const Transform* p = get_parent_transform();
        return p ? transform_detail::mul(p->get_lossy_scale(), scale) : scale;
    }

    inline Vector3 right() const { return get_world_rotation().rotate(Vector3::Right); }
    inline Vector3 left() const { return get_world_rotation().rotate(Vector3::Left); }
    inline Vector3 up() const { return get_world_rotation().rotate(Vector3::Up); }
    inline Vector3 down() const { return get_world_rotation().rotate(Vector3::Down); }
    inline Vector3 forward() const { return get_world_rotation().rotate(Vector3::Forward); }
    inline Vector3 backward() const { return get_world_rotation().rotate(Vector3::Backward); }

    Vector3 local_right() const { return rotation.normalized().rotate(Vector3::Right); }
    Vector3 local_up() const { return rotation.normalized().rotate(Vector3::Up); }
    Vector3 local_forward() const { return rotation.normalized().rotate(Vector3::Forward); }

    void set_forward(const Vector3& dir) {
        set_world_rotation(transform_detail::look_rotation(dir, up()));
    }
    void set_up(const Vector3& dir) {
        set_world_rotation(transform_detail::from_to_rotation(up(), dir) * get_world_rotation());
    }
    void set_right(const Vector3& dir) {
        set_world_rotation(transform_detail::from_to_rotation(right(), dir) * get_world_rotation());
    }

    Vector3 transform_point(const Vector3& local_point) const {
        Vector3 in_parent = position + rotation.normalized().rotate(transform_detail::mul(scale, local_point));
        const Transform* p = get_parent_transform();
        return p ? p->transform_point(in_parent) : in_parent;
    }
    Vector3 inverse_transform_point(const Vector3& world_point) const {
        const Transform* p = get_parent_transform();
        Vector3 in_parent = p ? p->inverse_transform_point(world_point) : world_point;
        return transform_detail::div(rotation.normalized().conjugate().rotate(in_parent - position), scale);
    }
    Vector3 transform_direction(const Vector3& local_dir) const {
        return get_world_rotation().rotate(local_dir);
    }
    Vector3 inverse_transform_direction(const Vector3& world_dir) const {
        return get_world_rotation().conjugate().rotate(world_dir);
    }
    Vector3 transform_vector(const Vector3& local_vec) const {
        return transform_point(local_vec) - transform_point(Vector3::Zero);
    }
    Vector3 inverse_transform_vector(const Vector3& world_vec) const {
        return inverse_transform_point(world_vec) - inverse_transform_point(Vector3::Zero);
    }

    void translate(const Vector3& delta, Space space = Space::Self) {
        Vector3 world_delta = space == Space::Self ? transform_direction(delta) : delta;
        set_world_position(get_world_position() + world_delta);
    }

    void rotate(const Vector3& axis, float angle, Space space = Space::Self) {
        Quaternion q = Quaternion::from_axis_angle(axis.normalized(), angle);
        if (space == Space::Self)
            rotation = (rotation.normalized() * q).normalized();
        else
            set_world_rotation(q * get_world_rotation());
    }

    void rotate(float pitch, float yaw, float roll, Space space = Space::Self) {
        Quaternion q = Quaternion::Euler(pitch, yaw, roll);
        if (space == Space::Self)
            rotation = (rotation.normalized() * q).normalized();
        else
            set_world_rotation(q * get_world_rotation());
    }

    void rotate_around(const Vector3& point, const Vector3& axis, float angle) {
        Quaternion q = Quaternion::from_axis_angle(axis.normalized(), angle);
        set_world_position(point + q.rotate(get_world_position() - point));
        set_world_rotation(q * get_world_rotation());
    }

    void look_at(const Vector3& target, const Vector3& world_up = Vector3::Up) {
        set_world_rotation(transform_detail::look_rotation(target - get_world_position(), world_up));
    }
    void look_at(const Transform& target, const Vector3& world_up = Vector3::Up) {
        look_at(target.get_world_position(), world_up);
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
