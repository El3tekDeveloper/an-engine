layout(std430, binding = 0) buffer SSBO {
    StyleBox style_boxs[];
};

uniform vec2 u_screen_size;
uniform vec4 u_box_transform; 

out vec4 v_bg_color;
out vec4 v_border_color;
out vec4 v_border_width;
out vec4 v_corner_radius;
out vec2 v_local_pos;
out vec2 v_half_size;

void main() {
    StyleBox box = style_boxs[gl_InstanceID];
    
    vec4 margins = box.expand_margins;
    
    vec2 base_pos = u_box_transform.xy;
    vec2 base_size = u_box_transform.zw;
    vec2 total_size = base_size + vec2(margins.x + margins.z, margins.y + margins.w);
    
    vec2 vertices[6] = vec2[6](
        vec2(0.0,          0.0),
        vec2(0.0,          total_size.y),
        vec2(total_size.x, 0.0),
        vec2(total_size.x, 0.0),
        vec2(0.0,          total_size.y),
        vec2(total_size.x, total_size.y)
    );

    vec2 local_pixel = vertices[gl_VertexID];

    vec2 screen_pos = base_pos - vec2(margins.x, margins.y) + local_pixel;

    gl_Position = vec4(
        (screen_pos.x / u_screen_size.x) * 2.0 - 1.0,
        1.0 - (screen_pos.y / u_screen_size.y) * 2.0,
        0.0, 
        1.0
    );

    v_half_size = total_size * 0.5;
    v_local_pos = local_pixel - v_half_size;

    v_bg_color = box.bg_color;
    v_border_color = box.border_color;
    v_corner_radius = box.corner_radius;
    v_border_width = box.border_width;
}
