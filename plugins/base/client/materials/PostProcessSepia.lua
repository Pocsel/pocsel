Client.Material.Register{
    materialName = "PostProcessSepia",
    geometryShader = "base:shaders/PostProcessSepia.fx",
    shadowMapShader = "base:shaders/PostProcessSepia.fx",
    hasAlpha = false,

    shader = {
        mask = Client.Texture("base:cubes/water/texture.lua"),
    },
    Update = nil
}
