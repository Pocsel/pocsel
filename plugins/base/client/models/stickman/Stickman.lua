Client.Material.Register{
    materialName = "Stickman",
    geometryShader = "base:shaders/BaseModel.fx",
    shadowMapShader = "base:shaders/BaseModel.fx",
    shader = {
        diffuseTexture = "base:models/stickman/stickman.png",
        testColorRed = -1
    },
    hasAlpha = false,
    Update = function (self, totalTime)
        self.shader.testColorRed = math.sin(totalTime) * 0.5 + 0.5
    end,
}

Client.Model.Register{

    modelName = "Stickman",

    file = "base:models/stickman/stickman.mqm"

}
