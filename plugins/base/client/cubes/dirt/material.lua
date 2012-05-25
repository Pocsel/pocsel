Client.CubeMaterial.Register{

    cubeMaterialName = "Dirt",

    textures = {
        top = "cubes/dirt/texture.png",
        bottom = "cubes/dirt/texture.png",
        left = "cubes/dirt/texture.png",
        right = "cubes/dirt/texture.png",
        front = "cubes/dirt/texture.png",
        back = "cubes/dirt/texture.png",
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
