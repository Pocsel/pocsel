Server.Entity.Register{

    entityName = "Init", -- mandatory

    Spawn = function(self) -- constructor
        print("Spawn()")
        self.i = 0
        self:Loop()
        for i = 0, math.random() * 8 do
            Srv.Entity.Spawn({ x = math.random() * 10, y = math.random() * 10, z = math.random() * 10 }, "Blob", self.id)
        end
    end,

    Die = function(self) -- destructor
        print("Die()")
    end,

    Save = function(self) -- save to database
        self.storage.i = self.i
        if not self.storage.nbSaves then
            self.storage.nbSaves = 1
        else
            self.storage.nbSaves = self.storage.nbSaves + 1
        end
        print("number of saves: ", self.storage.nbSaves)
        Srv.Entity.Kill(self.storage.nbSaves + 1)
        Srv.Entity.Kill(self.storage.nbSaves + 2)
        for i = 0, math.random() * 6 do
            Srv.Entity.Spawn({ x = math.random() * 10, y = math.random() * 10, z = math.random() * 10 }, "Blob", self.id)
        end
    end,

    Load = function(self) -- load from database
        print("Load()")
        self.i = self.storage.i
    end,

    Loop = function(self)
        self.i = self.i + 1
        Srv.Message.Later(5, self.id, "Loop")
    end,

    GetCounter = function(self)
        return self.i
    end,

}

Server.Entity.RegisterPositional{

    entityName = "Blob",

    Spawn = function(self, initEntity)
        self:Loop(initEntity)


        -- self.d = Srv.Doodad.Spawn("Pelleteuse", {"PelleteuseBase", "PelleteusePelle"})
        -- Srv.Doodad.RotateBody(self.d, "PelleteusePelle", Utils.Quat(5, 0, 0, 0))


    end,

    Loop = function(self, initEntity)
        Srv.Message.Later(5 + math.random() * 20, initEntity, "GetCounter", nil, self.id, "GotCounter")
    end,

    GotCounter = function(self, arg, result)
        print(result.ret)
        self:Loop(result.entityId)
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
