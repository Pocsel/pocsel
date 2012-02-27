effectName = "Coucou"

shader = "cubes/water/effect.cgfx"
parameters = {
    texture = "cubeTexture",
    gameTime = "gameTime",
    normalMap = "normalMap",
    reflection = {
        identifier = "reflectionTexture",
        update = function () -- todo: les arguments (un objet/table qui contient l'etat du jeu, camera, player, timers, etc...)
                
                RenderWorld()
            end
    }
}