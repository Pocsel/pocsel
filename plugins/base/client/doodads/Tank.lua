Client.Doodad.Register{
    doodadName = "Tank",
    Spawn = function(self)
        print("spawn tank")
        self.modelId = Client.Model.Spawn("base:Tank")
    end,
    Think = function(self)
    end,
}
