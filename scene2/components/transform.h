#pragma once
#include "core/math/quaternion.h"
#include "component.h"

struct [[export]] Transform : public Component {
    [[export]] Vector3 position = Vector3::Zero;
    [[export]] Quaternion rotation = Quaternion::Identity;
    [[export]] Vector3 scale = Vector3::One;
    
    inline Matrix4 get_local_matrix() const { return get_matrix(); }
private:
    Matrix4 get_matrix() const {
        Matrix4 T = Matrix4::translation(position);
        Matrix4 R = rotation.normalized().to_matrix();
        Matrix4 S = Matrix4::scale(scale);

        return T * R * S;
    }

    friend class RenderSystem;
};

#if __REFLECT_GENERATED__
#include ".generated/transform.generated.hxx"
#endif 
