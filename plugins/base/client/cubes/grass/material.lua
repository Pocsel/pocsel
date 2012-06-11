Client.CubeMaterial.Register{

    cubeMaterialName = "Grass",

    textures = {
        top = "base:cubes/grass/top.png",
        bottom = "base:cubes/dirt/texture.png",
        left = "base:cubes/grass/side.png",
        right = "base:cubes/grass/side.png",
        front = "base:cubes/grass/side.png",
        back = "base:cubes/grass/side.png",
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
