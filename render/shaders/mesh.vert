layout(std430, binding = 0) buffer vertices_ssbo {
    Vertex vertices[];
};
layout(std430, binding = 1) buffer indices_ssbo {
    uint indices[];
};

uniform uint index_offset;

out vec2 texture_coords;
out vec4 vertex_color;
out vec3 frag_position;
out vec3 frag_normal;
out vec3 frag_tangent;

uniform mat4 model;
uniform mat4 camera;

void main()
{
    uint index = indices[gl_VertexID + index_offset];
    Vertex vertex = vertices[index];

    gl_Position = camera * model * vec4(vertex.position, 1.0);
    frag_position = vec3(model * vec4(vertex.position, 1.0));
    texture_coords = vertex.uv;
    vertex_color = vertex.color;

    mat3 normal_matrix = transpose(inverse(mat3(model)));
    frag_normal = normalize(normal_matrix * vertex.normal);
    frag_tangent = normalize(normal_matrix * vertex.tangent);
}
