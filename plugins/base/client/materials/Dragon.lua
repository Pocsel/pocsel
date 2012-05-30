Client.Material.Register{
    materialName = "Dragon",
    hasAlpha = false, -- obligatoire pour savoir si la mati�re est transparente (la texture peut ne pas l'�tre mais le shader peut rendre transparent)
    geometryShader = "materials/DragonGeometry.fx",
    shadowMapShader = "materials/DragonShadowMap.fx",
    shininess = 15, -- Variable du shader (ayant le m�me nom)
    colorTint = Utils.Color.Create(0.3, 0.3, 0.3, 1), -- Possibilit� de changer ces variables directement dans les Doodads (exemple: self.model.meshes["head"].material.shininess = 5)
    texture = "materials/Dragon.png", -- Exemple: self.model.meshes["body"].material.texture = "DragonAnimatedHead"
}
