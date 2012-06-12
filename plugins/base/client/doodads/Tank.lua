Client.Doodad.Register{
    doodadName = "Tank",
    Spawn = function(self)
        self.model = Client.Model.Spawn(self.id, "Tank")
    end,
    Think = function(self)
    end,
}

Client.Model.Register{
    modelName = "Tank",
    file = "tank.mqm",
    angles = {
        Turret = {
            turret = 1
        },
        Cannon = {
            cannon = 1
        }
    }
}
