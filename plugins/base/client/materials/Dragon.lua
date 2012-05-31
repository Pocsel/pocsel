Client.Material.Register{
    materialName = "Dragon",
    hasAlpha = false, -- obligatoire pour savoir si la matière est transparente (la texture peut ne pas l'être mais le shader peut rendre transparent)
    geometryShader = "materials/DragonGeometry.fx",
    shadowMapShader = "materials/DragonShadowMap.fx",
    shaderVariables = {
        shininess = 15, -- Variable du shader (ayant le même nom)
        colorTint = Utils.Color.Create(0.3, 0.3, 0.3, 1), -- Possibilité de changer ces variables directement dans les Doodads (exemple: self.model.meshes["head"].material.shininess = 5)
        texture = "materials/Dragon.png", -- Exemple: self.model.meshes["body"].material.texture = "DragonAnimatedHead"
    }
    timeParameter = "time",
    isBlack = false,
    Update = function(self, totalTime)
        self.hasAlpha = true
        self.shaderVariables.time = totalTime
    end
}


self.model.meshes["body"].material.isBlack = self.isBlack

if self.isBlack then
    self.model.meshes.GetMesh("body").GetMaterial().colorTint = Utils.Color.Create(0.3, 0.3, 0.3, 1)
else
    self.model.meshes.GetMesh("body").GetMaterial().shaderVariables.colorTint = Utils.Color.Create(0.9, 0.9, 0.9, 1)
end