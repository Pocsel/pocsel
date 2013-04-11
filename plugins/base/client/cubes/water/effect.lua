Client.Material.Register{
    materialName = "Water",
    geometryShader = "base:cubes/water/effect.fx",
    shadowMapShader = "base:cubes/water/effect.fx",
    hasAlpha = true,

    shader = {
        cubeTexture = Client.Texture("base:cubes/water/texture.lua"),
        currentTime = 0,
    },
    Update = function(self, totalTime)
        self.shader.currentTime = totalTime
    end
}
