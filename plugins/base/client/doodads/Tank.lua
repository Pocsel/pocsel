Client.Doodad.Register{
    doodadName = "Tank",
    Spawn = function(self)
        print("spawn tank")
        --self.modelId = Client.Model.Spawn("base:Tank")
        --Client.Model.BindBoneToBodyNode(self.modelId, "bite", "kaka")
        modelId = Client.Model.Spawn("base:Tank")
        Client.Model.BindBoneToBodyNode(modelId, "bite", "bite")
        Client.Model.BindBoneToBodyNode(modelId, "bite", "Gun")
        Client.Model.BindBoneToBodyNode(modelId, "gun", "Gun")
        Client.Model.BindBoneToBodyNode(modelId, "turret", "Turret")
        Client.Model.BindBoneToBodyNode(modelId, "hull", "Hull")
    end,
    Think = function(self)
    end,
}
