
Client.Material.Register{
    materialName = "BaseChunk",
    geometryShader = "base:shaders/BaseChunk.fx",
    shadowMapShader = "base:shaders/BaseChunk.fx",
    hasAlpha = false,
    
    shader = {
        cubeTexture = "base:models/stickman/stickman.png",
    },
    Update = nil
}

local function RegisterSimpleCubeType(name, textureName)
    local material = Client.Material("base:BaseChunk", { cubeTexture = textureName })
    Client.CubeMaterial.Register{
        cubeMaterialName = name,
        materials = {
            top = material,
            bottom = material,
            left = material,
            right = material,
            front = material,
            back = material
        }
    }
end

local function RegisterCubeType(name, topTexture, sideTexture, bottomTexture)
    local sideMaterial = Client.Material("base:BaseChunk", { cubeTexture = sideTexture })
    Client.CubeMaterial.Register{
        cubeMaterialName = name,
        materials = {
            top = Client.Material("base:BaseChunk", { cubeTexture = topTexture }),
            bottom = Client.Material("base:BaseChunk", { cubeTexture = bottomTexture }),
            left = sideMaterial,
            right = sideMaterial,
            front = sideMaterial,
            back = sideMaterial
        }
    }
end

-- Dirt
RegisterSimpleCubeType("Dirt", "base:cubes/dirt/texture.png")
-- Grass
RegisterCubeType("Grass", "base:cubes/grass/top.png", "base:cubes/grass/side.png", "base:cubes/dirt/texture.png")
-- Iron
RegisterSimpleCubeType("Iron", "base:cubes/iron/texture.png")
-- Rock
RegisterSimpleCubeType("Rock", "base:cubes/rock/texture.png")
-- Water
RegisterSimpleCubeType("Water", "base:cubes/water/texture.lua")