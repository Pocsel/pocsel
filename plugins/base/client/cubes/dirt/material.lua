Client.CubeMaterial.Register{

    cubeMaterialName = "Dirt",

    textures = {
        top = "base:cubes/dirt/texture.png",
        bottom = "base:cubes/dirt/texture.png",
        left = "base:cubes/dirt/texture.png",
        right = "base:cubes/dirt/texture.png",
        front = "base:cubes/dirt/texture.png",
        back = "base:cubes/dirt/texture.png",
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
