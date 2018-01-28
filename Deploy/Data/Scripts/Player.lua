local Player = {};
Player.__index = Player;

function Player.New()
    local self = {};
    self.speed = 10.0;

    return setmetatable(self, Player);
end

setmetatable(Player, { __call = Player.New })

function Player:Finalize(entitySelf)
    self.transform = Game.ComponentSystem.GetTransform(entitySelf);
    
    if self.transform == nil then
        return false;
    end

    return true;
end

function Player:Update(entitySelf, timeDelta)
    -- Get the movement direction.
    local direction = Math.Vec3();

    if System.InputState.IsKeyboardKeyDown(System.KeyboardKeys["D"]) then
        direction.x = direction.x + 1.0;
    end

    if System.InputState.IsKeyboardKeyDown(System.KeyboardKeys["A"]) then
        direction.x = direction.x - 1.0;
    end

    if System.InputState.IsKeyboardKeyDown(System.KeyboardKeys["W"]) then
        direction.y = direction.y + 1.0;
    end

    if System.InputState.IsKeyboardKeyDown(System.KeyboardKeys["S"]) then
        direction.y = direction.y - 1.0;
    end

    -- Normalize the direction vector.
    if direction:LengthSqr() ~= 0.0 then
        direction = direction:Normalize();
    end

    -- Update the transform position.
    local position = self.transform:GetPosition();
    position = position + direction * self.speed * timeDelta;
    self.transform:SetPosition(position);
end

return Player;
