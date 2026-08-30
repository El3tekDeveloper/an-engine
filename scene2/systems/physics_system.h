#pragma once
#include <cmath>
#include <cstdint>
#include <unordered_set>
#include "core/math/vector3.h"
#include "core/os/time.h"
#include "scene2/components/component.h"
#include "scene2/components/transform.h"
#include "system.h"
#include "scene2/scene.h"
#include "scene2/registry.h"

struct [[export]] AABB : public Component {
    Vector3 half_extents = Vector3::One * 0.5f;

    Vector3 world_min = Vector3::Zero;
    Vector3 world_max = Vector3::Zero;

private:
    void update(const Transform& transform) {
        Vector3 scaled_extents = Vector3(
            half_extents.x * transform.scale.x,
            half_extents.y * transform.scale.y,
            half_extents.z * transform.scale.z
        );
        world_min = transform.position - scaled_extents;
        world_max = transform.position + scaled_extents;
    }

    bool overlaps(const AABB& other) const {
        return world_min.x <= other.world_max.x && world_max.x >= other.world_min.x &&
               world_min.y <= other.world_max.y && world_max.y >= other.world_min.y &&
               world_min.z <= other.world_max.z && world_max.z >= other.world_min.z;
    }

    friend class PhysicsSystem;
};

struct [[export]] RigidBody : public Component {
    [[export]] Vector3 velocity = Vector3::Zero;
    [[export]] float mass = 1.0f;
    [[export]] float gravity_scale = 1.0f;
    [[export]] float restitution = 0.0f;
    [[export]] bool is_static = false;
};

class SpatialHashGrid {
public:
    explicit SpatialHashGrid(float cell_size = 2.0f) : cell_size_(cell_size) {}

    void clear() {
        cells_.clear();
    }

    void insert(Entity entity, const AABB& aabb) {
        int32_t min_x = cell_coord(aabb.world_min.x);
        int32_t min_y = cell_coord(aabb.world_min.y);
        int32_t min_z = cell_coord(aabb.world_min.z);
        int32_t max_x = cell_coord(aabb.world_max.x);
        int32_t max_y = cell_coord(aabb.world_max.y);
        int32_t max_z = cell_coord(aabb.world_max.z);

        for (int32_t x = min_x; x <= max_x; ++x)
            for (int32_t y = min_y; y <= max_y; ++y)
                for (int32_t z = min_z; z <= max_z; ++z)
                    cells_[key(x, y, z)].push_back(entity);
    }

    template <typename Callback>
    void for_each_candidate_pair(Callback&& callback) {
        checked_pairs_.clear();
        for (auto& kv : cells_) {
            std::vector<Entity>& entities = kv.second;
            for (size_t i = 0; i < entities.size(); ++i) {
                for (size_t j = i + 1; j < entities.size(); ++j) {
                    Entity a = entities[i];
                    Entity b = entities[j];
                    uint64_t pair_key = make_pair_key(a, b);
                    if (!checked_pairs_.insert(pair_key).second) continue;
                    callback(a, b);
                }
            }
        }
    }

private:
    float cell_size_;
    std::unordered_map<int64_t, std::vector<Entity>> cells_;
    std::unordered_set<uint64_t> checked_pairs_;

    int32_t cell_coord(float v) const {
        return static_cast<int32_t>(std::floor(v / cell_size_));
    }

    static int64_t key(int32_t x, int32_t y, int32_t z) {
        auto to_u = [](int32_t v) -> uint64_t {
            return static_cast<uint64_t>(static_cast<uint32_t>(v)) & 0x1FFFFF;
        };
        return static_cast<int64_t>((to_u(x) << 42) | (to_u(y) << 21) | to_u(z));
    }

    static uint64_t make_pair_key(Entity a, Entity b) {
        uint32_t ia = ecs::entity::index(a);
        uint32_t ib = ecs::entity::index(b);
        if (ia > ib) std::swap(ia, ib);
        return (static_cast<uint64_t>(ia) << 32) | ib;
    }
};

class PhysicsSystem : public System {
public:
    virtual ~PhysicsSystem() = default;

    Vector3 gravity = Vector3(0.0f, -9.81f, 0.0f);
    int substeps = 4;
    
    float broadphase_cell_size = 2.0f;

    void update(Scene& scene) override {
        float dt = Time.delta_time() / (float)substeps;
        
        cached_entities_.clear();
        for (auto entity : scene.get_registry().view<Transform, RigidBody, AABB>()) {
            cached_entities_.push_back(entity);
        }

        if (grid_cell_size_ != broadphase_cell_size) {
            grid_ = SpatialHashGrid(broadphase_cell_size);
            grid_cell_size_ = broadphase_cell_size;
        }

        for (int step = 0; step < substeps; ++step) {
            integrate(scene, dt);
            refresh_bounds(scene);
            resolve_collisions(scene, grid_);
        }
    }

private:
    std::vector<Entity> cached_entities_;
    SpatialHashGrid grid_{broadphase_cell_size};
    float grid_cell_size_ = 2.0f;

    void integrate(Scene& scene, float dt) {
        for (auto entity : scene.get_registry().view<Transform, RigidBody>()) {
            RigidBody& body = scene.get_component<RigidBody>(entity);
            if (body.is_static) continue;

            Transform& transform = scene.get_component<Transform>(entity);

            body.velocity += gravity * body.gravity_scale * dt;
            transform.position += body.velocity * dt;
        }
    }

    void refresh_bounds(Scene& scene) {
        for (auto entity : scene.get_registry().view<Transform, AABB>()) {
            Transform& transform = scene.get_component<Transform>(entity);
            AABB& aabb = scene.get_component<AABB>(entity);
            aabb.update(transform);
        }
    }

    void resolve_collisions(Scene& scene, SpatialHashGrid& grid) {
        grid.clear();
        for (Entity entity : cached_entities_) {
            AABB& aabb = scene.get_component<AABB>(entity);
            grid.insert(entity, aabb);
        }

        grid.for_each_candidate_pair([&](Entity a, Entity b) {
            RigidBody& body_a = scene.get_component<RigidBody>(a);
            RigidBody& body_b = scene.get_component<RigidBody>(b);
            if (body_a.is_static && body_b.is_static) return;

            AABB& aabb_a = scene.get_component<AABB>(a);
            AABB& aabb_b = scene.get_component<AABB>(b);
            if (!aabb_a.overlaps(aabb_b)) return;

            Transform& transform_a = scene.get_component<Transform>(a);
            Transform& transform_b = scene.get_component<Transform>(b);

            Vector3 overlap(
                std::min(aabb_a.world_max.x, aabb_b.world_max.x) - std::max(aabb_a.world_min.x, aabb_b.world_min.x),
                std::min(aabb_a.world_max.y, aabb_b.world_max.y) - std::max(aabb_a.world_min.y, aabb_b.world_min.y),
                std::min(aabb_a.world_max.z, aabb_b.world_max.z) - std::max(aabb_a.world_min.z, aabb_b.world_min.z)
            );

            Vector3 normal = Vector3::Zero;
            float push = 0.0f;
            if (overlap.x <= overlap.y && overlap.x <= overlap.z) {
                normal.x = (transform_a.position.x < transform_b.position.x) ? -1.0f : 1.0f;
                push = overlap.x;
            } else if (overlap.y <= overlap.x && overlap.y <= overlap.z) {
                normal.y = (transform_a.position.y < transform_b.position.y) ? -1.0f : 1.0f;
                push = overlap.y;
            } else {
                normal.z = (transform_a.position.z < transform_b.position.z) ? -1.0f : 1.0f;
                push = overlap.z;
            }

            float total_mass = body_a.is_static ? 1.0f : (body_b.is_static ? 1.0f : (body_a.mass + body_b.mass));
            float ratio_a = body_a.is_static ? 0.0f : (body_b.is_static ? 1.0f : body_b.mass / total_mass);
            float ratio_b = body_b.is_static ? 0.0f : (body_a.is_static ? 1.0f : body_a.mass / total_mass);

            if (!body_a.is_static) transform_a.position += normal * push * ratio_a;
            if (!body_b.is_static) transform_b.position -= normal * push * ratio_b;

            float restitution = std::max(body_a.restitution, body_b.restitution);
            if (!body_a.is_static) {
                float v = body_a.velocity.dot(normal);
                if (v < 0.0f) body_a.velocity -= normal * v * (1.0f + restitution);
            }
            if (!body_b.is_static) {
                float v = body_b.velocity.dot(-normal);
                if (v < 0.0f) body_b.velocity -= (-normal) * v * (1.0f + restitution);
            }
        });
    }
};

#if __REFLECT_GENERATED__
#include ".generated/physics_system.generated.hxx"
#endif
