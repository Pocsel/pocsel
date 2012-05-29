Server.Entity.Register{

    entityName = "Init", -- mandatory

    Spawn = function(self) -- constructor
        print("Spawn()")
        Server.Message.Later(10, self.id, "SpawnBlob")
    end,

    Die = function(self) -- destructor
        print("Die()")
    end,

    Save = function(self) -- save to database
        print("Save()")
    end,

    Load = function(self) -- load from database
        print("Load()")
    end,

    SpawnBlob = function(self)
        Server.Entity.Spawn({ x = 67108864, y = 16777216 + 10, z = 67108864 }, "Blob")
        Server.Entity.Spawn({ x = 67108864, y = 16777216 + 12, z = 67108864 }, "Tank")
        Server.Message.Later(10, self.id, "SpawnBlob")
    end,

}

Server.Entity.RegisterPositional{

    entityName = "Blob",

    Spawn = function(self, initEntity)
        print("Spawn()")
        self.doodad = Server.Doodad.Spawn("TestDoodad")
        Server.Doodad.Rotate(self.doodad, angle)
        Server.Entity.Rotate(self.id, angle)
        Server.Message.Later(5, self.id, "CallTest")
        Server.Message.Later(10, self.id, "KillTest")
        self:Move()
    end,

    Die = function(self)
        print("Die()");
    end,

    Save = function(self)
        print("Save()")
    end,

    Load = function(self)
        print("Load()")
    end,

    CallTest = function(self)
        Server.Doodad.Call(self.doodad, "TestFunction", {})
        Server.Doodad.Call(self.doodad, "TestFunction2", {})
    end,

    KillTest = function(self)
        --Server.Doodad.Kill(self.doodad)
    end,

    Move = function(self)
        local pos = Server.Entity.GetPos(self.id)
        Server.Entity.SetPos(self.id, { x = pos.x + 0.1, y = pos.y, z = pos.z })
        Server.Message.Later(0.5, self.id, "Move")
        print("Move() ", pos.x, " ", pos.y, " ", pos.z)
    end,

}

-- Server.Body.Register{
--
--     bodyName = "monBody",
--
--     -- definitions bounding boxes
--
-- }
--
-- Client.Doodad.Register{
--
--     doodadName = "Chicken",
--
--     Spawn = function(self)
--         self.m = Client.Model.Spawn("Chicken")
--         self.s = Client.Sound.Spawn("CACA")
--     end,
--
-- }
