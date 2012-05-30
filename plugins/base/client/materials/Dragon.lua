Client.Material.Register{
    materialName = "Dragon",
    hasAlpha = false, -- obligatoire pour savoir si la matière est transparente (la texture peut ne pas l'être mais le shader peut rendre transparent)
    geometryShader = "materials/DragonGeometry.fx",
    shadowMapShader = "materials/DragonShadowMap.fx",
    shininess = 15, -- Variable du shader (ayant le même nom)
    colorTint = Utils.Color.Create(0.3, 0.3, 0.3, 1), -- Possibilité de changer ces variables directement dans les Doodads (exemple: self.model.meshes["head"].material.shininess = 5)
    texture = "materials/Dragon.png", -- Exemple: self.model.meshes["body"].material.texture = "DragonAnimatedHead"
}
