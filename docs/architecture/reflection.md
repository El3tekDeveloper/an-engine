# Reflection System

## Main Idea

The reflection system is built around a base `Type` class. First, we register the basic types such as primitive types and math types. These types store common information like size, alignment, and basic get/set functions.

To improve the architecture, every type has a `TypeKind` that describes what kind of type it is.

The registry itself is just an `std::unordered_map`:

- `std::type_index` is the key.
- `Type*` is the value.

This makes registering and getting types very simple.

The interesting part is handling types like pointers, references, arrays, and containers such as `std::vector`.

This can be done using C++ type traits like:

- `std::is_pointer_v`
- `std::is_reference_v`
- `std::is_array_v`

and custom traits for types like `std::vector`.

The `get_type<T>()` function checks these traits and returns the correct reflection type while keeping the original type information.

Using this approach, we can build more specialized types that inherit from `Type`, such as:

- `ClassType`
- `EnumType`
- `ArrayType`
- `PointerType`

For example, `ClassType` stores not only the type itself, but also its fields and functions.

---

## Reflected Classes

Writing all the reflection code by hand would be a lot of work.

Instead, the project uses **Clang LibTooling** to read the C++ source code. It can inspect classes, fields, functions, inheritance, and annotations while the project is being built.

The generator then creates files like:

```cpp
my_class.generated.hxx
```

These generated files contain all the registration code, so reflection stays synchronized with the source code automatically.

---

## Plugins

Plugins are a very important part of the reflection system.

The generator exposes hooks such as:

- `on_class_begin`
- `on_class_end`
- `on_field`
- `on_function`

A plugin can generate any code it needs inside the generated files.

Plugins can also decide which classes they work on. For example, they can check if a class inherits from a specific base class before generating anything.

Every reflected object can also store metadata using:

```cpp
std::unordered_map<std::string, std::any>
```

This makes plugins, fields, and functions much more useful for different systems.

---

## Annotations

The reflection system does not rely on macros.

Instead, it uses Clang annotations, which makes the code look cleaner and more natural.

For example:

```cpp
class [[clang::annotate("reflect-export")]] Player
{
};
```
The generator simply looks for these annotations while parsing the source code.


We can use `#define` to make it more suitable and concise
like ```#define export clang::annotate("reflect-export")```

So it will be:
```cpp
class [[export]] Player
{
};
```

and we can make useful things using annotations and plugins like:
```#define range(mn, mx) clang::annotate("reflect-range;" #mn "," #mx)```

---

## Building

The project uses **Meson** as its build system.

Meson makes it easy to organize the project with subdirectories and custom build targets.

The reflection generator can be built as a separate executable or a shared library, then run during the build process before the main project is compiled.

