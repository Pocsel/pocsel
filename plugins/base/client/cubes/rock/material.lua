Client.CubeMaterial.Register{

    cubeMaterialName = "Rock",

    textures = {
        top = "cubes/rock/texture.png",
        bottom = "cubes/rock/texture.png",
        left = "cubes/rock/texture.png",
        right = "cubes/rock/texture.png",
        front = "cubes/rock/texture.png",
        back = "cubes/rock/texture.png",
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
