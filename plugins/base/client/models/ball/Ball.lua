Client.Material.Register{
    materialName = "Ball",
    geometryShader = "base:models/ball/ball.fx",
    shadowMapShader = "base:models/ball/ball.fx",
    shader = {
        diffuseTexture = Client.Texture("base:models/ball/ball.png"),
    },
    hasAlpha = false,
    Update = nil
}

Client.Model.Register{

    modelName = "Ball",

    file = "base:models/ball/ball.mqm"

}
