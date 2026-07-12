---@diagnostic disable: undefined-global -- Turn off warnings
local frequency = 1.5
local amplitude = 2

local time = 0
local start_y = 0

local function generate_random_color()
    math.randomseed(os.time())
    return Color.new(
        0.1 + math.random() * 0.9,
        0.1 + math.random() * 0.9,
        0.1 + math.random() * 0.9,
        1.0
    )
end

function awake()
    transform.position = Vector3.new(0, -3, -1)
    transform.scale = Vector3.new(50, 0.2, 50)
    start_y = transform.position.y

    game_object:add_component(AABB);
    game_object:add_component(RigidBody).is_static = true;

    if not game_object:has_component(MeshRenderer) then
        local mr = game_object:add_component(MeshRenderer)
        mr.mesh = Mesh.Cube

        local material = ResourceManager.create_material()
        material.albedo_factor = generate_random_color()
        mr.add_material(material)
    end
end

function update()
    time = time + Time.delta_time()
    local wave = math.sin(time * frequency) * amplitude
    transform.position.y = start_y + wave
end
