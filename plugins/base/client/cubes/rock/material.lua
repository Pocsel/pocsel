Client.CubeMaterial.Register{

    cubeMaterialName = "Rock",

    textures = {
        top = "base:cubes/rock/texture.png",
        bottom = "base:cubes/rock/texture.png",
        left = "base:cubes/rock/texture.png",
        right = "base:cubes/rock/texture.png",
        front = "base:cubes/rock/texture.png",
        back = "base:cubes/rock/texture.png",
    },

    effects = {
        top = "base:BaseChunk",
        bottom = "base:BaseChunk",
        left = "base:BaseChunk",
        right = "base:BaseChunk",
        front = "base:BaseChunk",
        back = "base:BaseChunk",
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
