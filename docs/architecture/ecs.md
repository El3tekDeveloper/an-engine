# ECS System

## Main Idea

The ECS module (part of the **Scene** module) implements a lightweight, archetype-less Entity-Component-System. There is no separate "archetype table" or chunked storage — instead, every component type gets its own dense pool, and each entity carries a bitmask describing which components it currently owns.

An `Entity` is not an object; it is just a 64-bit integer (`ecs::Entity`) that packs together an **index** and a **version**:

```cpp
constexpr Entity make(uint32_t index, uint32_t version) {
    return (static_cast<Entity>(index) << ENTITY_SHIFT) |
           static_cast<Entity>(version);
}
```

The version half exists so that when an entity slot is recycled, old (stale) entity handles can be detected as invalid rather than silently pointing at whatever new entity now occupies that slot.

Everything in this layer lives in the `ecs` namespace (`ecs.h`, `registry.h`), while `Scene`, `Component`, `System`, and `GameObject` sit at global scope and pull the namespace in with `using namespace ecs;`.

---

## Entity

Defined in `ecs.h`, under `ecs::entity`:

- `index(entity)` — extracts the slot index (top 32 bits).
- `version(entity)` — extracts the version (bottom 32 bits).
- `is_valid(entity)` — an entity is invalid if its index equals `UINT32_MAX`.
- `INVALID` — the sentinel entity, `make(UINT32_MAX, 0)`.
- `next_index(count)` — the index a newly appended entity would receive.

`MAX_ENTITIES` (100,000) and `MAX_COMPONENETS` (64) bound the system: component pools are sized for the maximum entity count up front, and the component mask is a fixed-size `std::bitset<64>`.

---

## Component

`Component` (in `component.h`) is the base class every gameplay component inherits from:

```cpp
struct [[export]] Component {
    virtual ~Component() = default;
    [[export]] bool active = true;

    virtual inline std::string get_type_name() const {
        return type_name(*this);
    }
};
```

The `[[export]]` annotations tie it into the reflection system (see `reflection.md`). the base class itself, and its `active` field, are reflected.

Each concrete component type is assigned a runtime id lazily via a static counter:

```cpp
template<typename T>
ComponentType type_id() {
    static ComponentType id = get_unique_id();
    return id;
}
```

This id is what indexes into both a `Scene`'s component pools and an entity's `ComponentMask` (`std::bitset<MAX_COMPONENETS>`).

---

## Component Pool

A `ComponentPool` is a raw, untyped byte buffer sized to hold `MAX_ENTITIES` elements of one component type:

```cpp
struct ComponentPool {
    ComponentPool(size_t element_size) {
        m_element_size = element_size;
        m_data = static_cast<std::byte*>(::operator new(element_size * MAX_ENTITIES));
    }

    inline void* get(size_t index) {
        return m_data + index * m_element_size;
    }
    ...
};
```

`Scene` keeps one `ComponentPool` per component type (`std::vector<std::unique_ptr<ComponentPool>>`, indexed by `type_id<T>()`), allocating it lazily the first time that component type is assigned. Components are constructed in place with placement-`new` directly into the entity's slot in the pool, so storage stays contiguous per type — no per-component heap allocation.

> The code notes a possible future improvement here: replacing the manual `operator new`/`operator delete` pair with `std::pmr::monotonic_buffer_resource`.

---

## Scene

`Scene` (`scene.h`) is the owner of ECS state for a level/world. It holds:

- `entities` — `std::vector<std::pair<Entity, ComponentMask>>`, the source of truth for which entities exist and what they own.
- `free_entities` — recycled entity slots, reused (with a bumped version) on the next `create_entity()`.
- `component_pools` — one `ComponentPool` per component type, indexed by `type_id<T>()`.
- `systems` — the list of `System` instances driving per-frame logic.
- `registry` — an `ecs::Registry` bound to this scene, offering a more ergonomic query API on top of the same data.

Core template operations:

```cpp
template<typename T, typename... TArgs>
T& Scene::assign_component(Entity id, TArgs&&... args);

template<typename T>
T& Scene::get_component(Entity id);

template<typename T>
void Scene::remove_component(Entity id);
```

`assign_component` grows the pool vector if needed, lazily creates the pool for `T`, placement-constructs the component, and sets the corresponding bit in the entity's `ComponentMask`. `remove_component` currently just clears the bit (the pool slot is not destructed/reclaimed).

`Scene` also drives the frame: `initialize()`, `update()`, and `draw(RenderData&)` iterate `systems`, and `add_system()` / `get_system<T>()` manage them (`get_system` does a `dynamic_cast` scan).

Scripting is layered directly on top of components: `add_script<T>()` assigns a `ScriptComponent` wrapping the script instance, and calls `awake()` immediately if the scene is already initialized.

---

## GameObject

`GameObject` (`game_object.h`) is a thin, copyable handle pairing an `Entity` with the `Scene*` that owns it, so gameplay code doesn't have to pass both around separately:

```cpp
struct GameObject {
    Entity id;
    Scene* scene;

    template<typename T, typename... TArgs>
    T& add_component(TArgs&&... args) { return scene->assign_component<T>(id, ...); }
    ...
    Transform& transform() const { return scene->get_component<Transform>(id); }
};
```

It forwards `add_component` / `get_component` / `remove_component` / `add_script` straight to the owning `Scene`, and offers a `transform()` shortcut since nearly every object has one.

---

## System

`System` (`system.h`) is the base class for per-frame logic that operates across many entities:

```cpp
class System {
    virtual void awake(Scene& scene) {};
    virtual void start(Scene& scene) {};
    virtual void update(Scene& scene) {};
    virtual void draw(Scene& scene, RenderData& render_data) {};
    ...
};
```

`update_internal` is private and only callable by `Scene` (declared `friend`): it guarantees `start()` runs exactly once, right before the first `update()`.

---

## Registry & Views

`ecs::Registry` (`registry.h`) wraps a `Scene&` and exposes a query-oriented API more typical of ECS libraries, sitting alongside the more direct `Scene::assign_component` / `get_component` style:

```cpp
template<typename... ComponentType>
bool has(Entity e) const;

template<typename T, typename... Args>
T& add(Entity e, Args&&... args);

template<typename... ComponentType>
View<ComponentType...> view();
```

It also adds bookkeeping helpers the raw `Scene` API doesn't: `valid(e)`, `mask_of(e)`, `alive_count()`, and `each(fn)` to iterate every alive entity.

`View<ComponentType...>` builds a `ComponentMask` from the requested component types at construction (or matches every entity if no types are given), and its `Iterator` walks `Scene::get_entities()`, skipping any slot whose entity is invalid or whose mask doesn't contain all requested components. This is what a system's `for (auto e : registry.view<Transform, Velocity>())` loop compiles down to.

---

## Component Ops & Reflection Integration

Because `Scene::assign_component<T>` / `get_component<T>` / `remove_component<T>` are templates, code that only has a *reflected* type (e.g. a `TypeClass*` from the editor or a serializer) has no way to call them directly — the concrete `T` isn't known at compile time in that context. `ComponentTypeOps` bridges that gap with type-erased function pointers:

```cpp
struct ComponentTypeOps {
    Component* (*add)(Scene& scene, Entity id) = nullptr;
    Component* (*get)(Scene& scene, Entity id) = nullptr;
    bool (*has)(Scene& scene, Entity id) = nullptr;
    void (*remove)(Scene& scene, Entity id) = nullptr;
};
```

`component_ops_registry()` maps each reflected `TypeClass*` to its `ComponentTypeOps`, and `find_component_ops(TypeClass*)` is the lookup helper.

Filling this registry is fully automatic, mirroring how the reflection generator produces registration code for every `[[export]]`-annotated class (see `reflection.md`). `ComponentOpsPlugin` is a `GeneratorPlugin` that hooks `on_class_end`: for every reflected class, it walks the base-class chain looking for `Component`, and if found, emits code into that class's `.generated.hxx` file which registers a `ComponentTypeOps` full of lambdas calling `assign_component<T>` / `get_component<T>` / a mask test / `remove_component<T>` for that concrete type.

The practical effect: marking a component `[[export]]` is enough — it is automatically wired into the generic, type-erased component API with zero hand-written glue code, the same way its fields and functions are automatically wired into the reflection system.

---

## Roadmap

The goal is not to match a general-purpose library like EnTT feature-for-feature. The priority is a stable baseline first, with each of the following added only once the previous step is solid, no step starts until the current state of the ECS is proven out:

1. **Stable baseline** — the dense-pool-per-component-type + bitmask design described above ships and stabilizes first.
2. **Archetypes** — group entities that share the same component set into contiguous storage, so a query only touches tightly-packed, relevant memory instead of scanning every entity.
3. **Chunk allocator** — a paged/chunked memory allocator backing archetype storage, replacing today's fixed `MAX_ENTITIES`-sized allocation per component pool.
4. **Sparse Set** — a packed sparse-set storage strategy for components that don't fit the archetype model well (frequently added/removed or rarely-used components).
5. **Query Optimizer** — caching and optimizing `View` / `Registry::view()` queries so repeated iteration over the same component combination doesn't re-scan the entity list each time.
6. **Job System** — parallelizing `System::update()` across entities/archetypes, introduced last since it needs a settled storage model to reason about safe concurrent access.
