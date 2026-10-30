uniform mat4 u_camera;
uniform vec4 u_rect; // xy = center, zw = size (line: xy = from, zw = to)
uniform float u_line_thickness;
uniform int u_circle_segments;
uniform int u_type;

const vec2 wire_rect[4] = vec2[](
    vec2(-0.5, -0.5),
    vec2( 0.5, -0.5),
    vec2( 0.5,  0.5),
    vec2(-0.5,  0.5)
);

const vec2 rect[6] = vec2[6](
    vec2(-0.5, -0.5), vec2(0.5, -0.5), vec2(0.5, 0.5),
    vec2(-0.5, -0.5), vec2(0.5, 0.5), vec2(-0.5, 0.5)
);


void main() {
    vec2 p;
    
    if (u_type == 0) p = u_rect.xy + wire_rect[gl_VertexID] * u_rect.zw;
    else if (u_type == 1) p = u_rect.xy + rect[gl_VertexID] * u_rect.zw;
    else if (u_type == 2) {
        vec2 from = u_rect.xy;
        vec2 to = u_rect.zw;
        
        vec2 direction = normalize(to - from);
        vec2 normal = vec2(-direction.y, direction.x) * (u_line_thickness * 0.05);

        const vec2 vertices[6] = vec2[](
            vec2(0.0, -1.0),
            vec2(1.0, -1.0),
            vec2(1.0,  1.0),
            vec2(0.0, -1.0),
            vec2(1.0,  1.0),
            vec2(0.0,  1.0)
        );

        vec2 v = vertices[gl_VertexID];
        p = mix(from, to, v.x) + normal * v.y;
    }
    else if (u_type == 3) {
        float angle = float(gl_VertexID / 2) / float(u_circle_segments) * 6.28318530718;
        float r = u_rect.z + (float(gl_VertexID % 2) - 0.5) * u_line_thickness * 0.05;
        p = u_rect.xy + vec2(cos(angle), sin(angle)) * r;
    }
    else if (u_type == 4) {
        float angle = float(gl_VertexID - 1) / float(u_circle_segments) * 6.28318530718;
        float r = gl_VertexID == 0 ? 0.0 : u_rect.z;
        p = u_rect.xy + vec2(cos(angle), sin(angle)) * r;
    }

    gl_Position = u_camera * vec4(p, 0.0, 1.0);
}
