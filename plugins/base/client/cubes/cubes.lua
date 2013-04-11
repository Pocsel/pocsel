require "base:materials/BaseChunk.lua"
require "base:cubes/water/effect.lua"
local nombreBonus = require "base:materials/BaseChunk.lua"

local function RegisterSimpleCubeType(name, textureName)
    local material = Client.Material("base:BaseChunk", { cubeTexture = Client.Texture(textureName) })
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
    local sideMaterial = Client.Material("base:BaseChunk", { cubeTexture = Client.Texture(sideTexture) })
    Client.CubeMaterial.Register{
        cubeMaterialName = name,
        materials = {
            top = Client.Material("base:BaseChunk", { cubeTexture = Client.Texture(topTexture) }),
            bottom = Client.Material("base:BaseChunk", { cubeTexture = Client.Texture(bottomTexture) }),
            left = sideMaterial,
            right = sideMaterial,
            front = sideMaterial,
            back = sideMaterial
        }
    }
end

local function RegisterSimpleCubeTypeWithMaterial(name, textureName, materialName)
    local material = Client.Material(materialName, { cubeTexture = Client.Texture(textureName) })
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


-- Dirt
RegisterSimpleCubeType("Dirt", "base:cubes/dirt/texture.png")
-- Grass
RegisterCubeType("Grass", "base:cubes/grass/top.png", "base:cubes/grass/side.png", "base:cubes/dirt/texture.png")
-- Iron
RegisterSimpleCubeType("Iron", "base:cubes/iron/texture.png")
-- Rock
RegisterSimpleCubeType("Rock", "base:cubes/rock/texture.png")
-- Water
RegisterSimpleCubeTypeWithMaterial("Water", "base:cubes/water/texture.lua", "base:Water")
