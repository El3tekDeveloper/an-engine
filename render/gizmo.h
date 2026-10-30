#pragma once
#include "core/math/vector2.h"
#include "core/math/color.h"
#include <cstdint>
#include <variant>

enum class GizmoShape : uint8_t {
    WireRect,
    Rect,
    Line,
    WireCircle,
    Circle
};

struct GizmoCommand {
    struct Rect {
        Vector2 pos;
        Vector2 size;
    };
    
    struct Line {
        Vector2 from;
        Vector2 to;
        float thickness;
    };

    struct Circle {
        Vector2 pos;
        float radius;
        float thickness;
        int segments;
    };

    std::variant<Rect, Line, Circle> props;
    GizmoShape shape;
    Color color;
};

struct Gizmo {
    static void draw_wire_rect(Vector2 pos, Vector2 size) {
        commands.push_back({.props = GizmoCommand::Rect{pos, size}, .shape = GizmoShape::WireRect, .color = color});
    }

    static void draw_rect(Vector2 pos, Vector2 size) {
        commands.push_back({.props = GizmoCommand::Rect{pos, size}, .shape = GizmoShape::Rect, .color = color});
    }

    static void draw_line(Vector2 from, Vector2 to, float thickness = 0.2) {
        commands.push_back({.props = GizmoCommand::Line{from, to, thickness}, .shape = GizmoShape::Line, .color = color});
    }

    static void draw_wire_circle(Vector2 pos, float radius, float thickness = 0.2, int segments = 64) {
        commands.push_back({.props = GizmoCommand::Circle{pos, radius, thickness, segments}, .shape = GizmoShape::WireCircle, .color = color});
    }

    static void draw_circle(Vector2 pos, float radius, int segments = 64) {
        commands.push_back({.props = GizmoCommand::Circle{pos, radius, 0.0, segments}, .shape = GizmoShape::Circle, .color = color});
    }

    static const std::vector<GizmoCommand>& get_commands() { return commands; }
    static void clear() { commands.clear(); }

    static inline Color color = Color::White;

private:
    static inline std::vector<GizmoCommand> commands;
};
