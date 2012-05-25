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
