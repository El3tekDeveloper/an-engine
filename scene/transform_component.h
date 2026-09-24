#pragma once
#include "component.h"
#include "core/math/matrix4.h"
#include "core/math/vector3.h"
#include "core/math/quaternion.h"

class [[export]] Transform : public Component {
public:
    [[export]]
    Vector3 position = Vector3::Zero;
    [[export]]
    Quaternion rotation = Quaternion::Identity;
    [[export]]
    Vector3 scale = Vector3::One;
    
    Matrix4 get_matrix() const {
        Matrix4 T = Matrix4::translation(position);
        Matrix4 R = rotation.normalized().to_matrix();
        Matrix4 S = Matrix4::scale(scale);

        return T * R * S;
    }
};

