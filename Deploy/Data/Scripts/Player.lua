local Player = {}
Player.__index = Player

function Player.New()
    local self = {}
    self.once = false

    return setmetatable(self, Player)
end

setmetatable(Player, { __call = Player.New })

function Player:Finalize(entitySelf)
    Log("Player:Finalize() script has been called.");

    return true
end

function Player:Update(entitySelf, timeDelta)
    if self.once == false then
        Log("Player:Update() script has been called.");
        self.once = true
    end
    
    if InputState.IsKeyboardKeyDown(KeyboardKeys["Escape"], false) then
        Log("Player:Update() - Escape has been pressed.");
    end
end

return Player
