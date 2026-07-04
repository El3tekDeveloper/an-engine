---@diagnostic disable: undefined-global -- Turn off warnings
local speed = 2.0
local dir = 1

function awake()
    if not game_object:has_component(MeshRenderer) then
        local mr = game_object:add_component(MeshRenderer)
        mr.mesh = Mesh.Cube
        local material = ResourceManager.create_material()
        material:set_albedo(ResourceManager.get_texture("assets/textures/2k_earth_daymap.jpg"))
        mr.add_material(material)
    end
end

function update()
    transform.position = transform.position + Vector3.new((dir * speed) * Time.delta_time(), 0, 0)
    transform.rotation = Quaternion.Euler(0, 45 * Time.time(), 0)
    transform.scale = Vector3.new(1, 1.5, 1)

    local x = transform.position.x
    if x >= 4 then
        dir = -1
    elseif x <= -4 then
        dir = 1
    end
end
