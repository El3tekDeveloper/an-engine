---@diagnostic disable: undefined-global -- Turn off warnings

local gravity = -9.8
local jump_force = 2.65

local current_velocity = 0.0

local cooldown_timer = 0.0
local cooldown_interval = 0.25

local sprite
local sprite_rect
local anim_time = 0.0

function sprite_x_animation(rect, animation_time, frame_size, frames_count, speed, start_frame)
    start_frame = start_frame or 0

    local start_x = start_frame * frame_size
    local end_x = start_x + frame_size * frames_count

    if rect.x < start_x or rect.x >= end_x then
        rect.x = start_x
    end

    animation_time = animation_time + Time.delta_time()

    if animation_time >= speed then
        animation_time = 0.0
        rect.x = rect.x + frame_size

        if rect.x >= end_x then
            rect.x = start_x
        end
    end

    return animation_time
end

function awake()
    sprite = Sprite.new()
    sprite:load("assets://sprites/Fish.png")
    sprite.rect = SpriteRect.new(0, 0, 64, 64)

    local sr = game_object:add_component(SpriteRenderer)
    sr.sprite = sprite
    sr.flip_x = true
end

function update()
    cooldown_timer = cooldown_timer + Time.delta_time();
    current_velocity = current_velocity + (gravity * Time.delta_time());

    local jump_pressed = Input.is_mouse_button_just_pressed(MouseButton.Left) or Input.is_key_just_pressed(Key.Space)
    if jump_pressed and cooldown_timer >= cooldown_interval then
        current_velocity = jump_force
        cooldown_timer = 0.0
    end

    transform.position.y = transform.position.y + current_velocity * Time.delta_time()
    transform.rotation.euler.y = current_velocity * 3

    if current_velocity > 0 then
        anim_time = sprite_x_animation(sprite.rect, anim_time, 64, 3, 0.2, 4)
    else
        anim_time = sprite_x_animation(sprite.rect, anim_time, 64, 3, 0.2, 0)
    end
end
