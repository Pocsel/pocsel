Client.CubeMaterial.Register {

    cubeMaterialName = "Iron",

    textures = {
        top = "base:cubes/iron/top.png",
        bottom = "base:cubes/iron/bottom.png",
        left = "base:cubes/iron/side.png",
        right = "base:cubes/iron/side.png",
        front = "base:cubes/iron/side.png",
        back = "base:cubes/iron/side.png",
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
