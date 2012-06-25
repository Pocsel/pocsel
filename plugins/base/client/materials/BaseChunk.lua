Client.Material.Register{
    materialName = "BaseChunk",
    geometryShader = "base:shaders/BaseChunk.fx",
    shadowMapShader = "base:shaders/BaseChunk.fx",
    hasAlpha = false,

    shader = {
        cubeTexture = Client.Texture("base:cubes/iron/texture.png"),
    },
    Update = nil
}
