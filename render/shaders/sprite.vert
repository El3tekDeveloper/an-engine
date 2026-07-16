const vec2 quad_corners[6] = vec2[6](
    vec2(0.0, 0.0), vec2(1.0, 0.0), vec2(1.0, 1.0),
    vec2(0.0, 0.0), vec2(1.0, 1.0), vec2(0.0, 1.0)
);

uniform mat4 model;
uniform mat4 camera;

uniform vec4 uv_rect;

uniform vec4 sprite_transform;

out vec2 texture_coords;

void main()
{
    vec2 corner = quad_corners[gl_VertexID];

    vec2 local_position = corner * sprite_transform.zw + sprite_transform.xy;

    gl_Position = camera * model * vec4(local_position, 0.0, 1.0);
    texture_coords = mix(uv_rect.xy, uv_rect.zw, corner);
}
