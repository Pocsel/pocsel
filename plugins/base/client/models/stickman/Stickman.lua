Client.Material.Register{
    materialName = "Stickman",
    geometryShader = "base:shaders/BaseModel.fx",
    shadowMapShader = "base:shaders/BaseModel.fx",
    shader = {
        diffuseTexture = Client.Texture("base:models/stickman/stickman.png"),
    },
    hasAlpha = false,
    Update = nil
}

Client.Model.Register{

    modelName = "Stickman",

    file = "base:models/stickman/stickman.mqm"

}
