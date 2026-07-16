uniform sampler2D sprite_texture;
uniform vec4 tint_color;

layout(location = 0) out vec4 frag_color;

in vec2 texture_coords;

void main()
{
    vec4 color = texture(sprite_texture, texture_coords) * tint_color;
    if (color.a <= 0.001) discard;
    frag_color = color;
}
