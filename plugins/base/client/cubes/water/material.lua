Client.CubeMaterial.Register{

    cubeMaterialName = "Water",

    textures = {
        top = "cubes/water/texture.lua",
        bottom = "cubes/water/texture.lua",
        left = "cubes/water/texture.lua",
        right = "cubes/water/texture.lua",
        front = "cubes/water/texture.lua",
        back = "cubes/water/texture.lua",
    },

    effects = {
        top = "BaseChunk",
        bottom = "BaseChunk",
        left = "BaseChunk",
        right = "BaseChunk",
        front = "BaseChunk",
        back = "BaseChunk",
    },

    effectParameters = {
        top = { diffuseColor = Utils.Color.Create(0x777777) },
        bottom = { diffuseColor = Utils.Color.Create(0x007700) },
        left = { diffuseColor = Utils.Color.Create(0x007777) },
        right = { diffuseColor = Utils.Color.Create(0x777700) },
        front = { diffuseColor = Utils.Color.Create(0x770000) },
        back = { diffuseColor = Utils.Color.Create(0x000077) },
    }

}

-- Client.CubeMaterial.Register{
-- 
--     cubeMaterialName = "Water",
-- 
--     material = { -- Optionnel, prend le nom du "CubeMaterial" pour toutes les faces
--         top = "Water",
--         bottom = "Water",
--         left = "Water",
--         right = "Water",
--         front = "Water",
--         back = "Water",
--     },
-- }

-- Client.Material.Water{
--     materialName = "Water",
--     hasAlpha = true,
--     geometryShader = "effects/BaseChunk.fx",
--     shadowMapShader = "effects/BaseChunkShadowMap.fx",
--     shininess = 15,
--     texture = "cubes/water/texture.lua",
-- }