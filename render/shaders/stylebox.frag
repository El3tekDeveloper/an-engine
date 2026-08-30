layout(location = 0) out vec4 frag_color;

in vec4  v_bg_color;
in vec4  v_border_color;
in vec2  v_local_pos;
in vec2  v_half_size;
in vec4  v_corner_radius; // x=top-left, y=top-right, z=bottom-right, w=bottom-left
in vec4  v_border_width;  // x=left, y=top, z=right, w=bottom

float sdf_rounded_rect(vec2 p, vec2 half_size, float r) {
    vec2 d = abs(p) - half_size + r;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - r;
}

void main() {
    float r = v_local_pos.x < 0.0
        ? (v_local_pos.y < 0.0 ? v_corner_radius.x : v_corner_radius.w)
        : (v_local_pos.y < 0.0 ? v_corner_radius.y : v_corner_radius.z);

    float radius = clamp(r, 0.0, min(v_half_size.x, v_half_size.y));

    float dist = sdf_rounded_rect(v_local_pos, v_half_size, radius);
    float aa   = fwidth(dist);

    float outer_mask = 1.0 - smoothstep(-aa, aa, dist);
    if (outer_mask <= 0.0) discard;

    vec2 border_min = v_border_width.xy; // left, top
    vec2 border_max = v_border_width.zw; // right, bottom
    
    vec2 inner_half   = v_half_size - (border_min + border_max) * 0.5;
    vec2 inner_offset = (border_max - border_min) * 0.5;

    float avg_border  = (v_border_width.x + v_border_width.y + v_border_width.z + v_border_width.w) * 0.25;
    float inner_radius = max(0.0, radius - avg_border);

    float fill_dist = sdf_rounded_rect(v_local_pos - inner_offset, max(inner_half, vec2(0.0)), inner_radius);
    float fill_mask = 1.0 - smoothstep(-aa, aa, fill_dist);

    vec4 final_color = mix(v_border_color, v_bg_color, fill_mask);
    
    frag_color = final_color * outer_mask;
}
