local Color = Utils.Color

Client.Material.Register{
    materialName = "Dragon",
    geometryShader = "materials/DragonGeometry.fx",
    shadowMapShader = "materials/DragonShadowMap.fx",

    shader = {
        shininess = 15, -- Variable du shader (ayant le même nom)
        colorTint = Utils.Color(0.3, 0.3, 0.3, 1), -- Possibilité de changer ces variables directement dans les Doodads (exemple: self.model.meshes["head"].material.shininess = 5)
        texture = "materials/Dragon.png", -- Exemple: self.model.meshes["body"].material.texture = "DragonAnimatedHead"
    }
    -- variables lua:
    hasAlpha = false, -- obligatoire pour savoir si la matière est transparente (la texture peut ne pas l'être mais le shader peut rendre transparent)
    timeParameter = "time",
    isBlack = false,
    Update = function(self, totalTime)
        self.hasAlpha = true
        self.shader.time = totalTime
        self.shader.bitePosition = Vector3(5, 6, 7) * 3 + Vector3({1, 5, 2})-- )
    end
}

-----------------------------------


-- Vector3 = {
--     "__call" = function (self, x, y, z)
--             return ___Vector3(x, y, z)
--         end
-- }
-- 
-- Vector3(1, 2, 3)
-- 
-- function ___Vector3(a, b, c)
--     v = {x = a, y = b, z = c}
--     setmetatable(v, UneMetableGlobaleVector3)
--     return v
-- end
-- 
-- a = { x = 15, y = 1, z = 45 }
-- a.x = a.x + 5
-- a.y = a.z
-- a.z = a.y
-- 
-- Vector3(a)

-- Des exemples:
-- self.model.meshes["body"].material.isBlack = self.isBlack
-- 
-- if self.isBlack then
--     self.model.meshes.GetMesh("body").GetMaterial().colorTint = Utils.Color.Create(0.3, 0.3, 0.3, 1)
-- else
--     self.model.meshes.GetMesh("body").GetMaterial().shaderVariables.colorTint = Utils.Color.Create(0.9, 0.9, 0.9, 1)
-- end