Server.Entity.Register{

    entityName = "Init", -- mandatory

    Spawn = function(self) -- constructor
        print("Spawn()")
        Server.Message.Later(20, self.id, "SpawnBlob")
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
    end,

}

Server.Entity.RegisterPositional{

    entityName = "Blob",

    Spawn = function(self)
        self.move = { x = -0.5+math.random(), y = -0.5+math.random(), z = -0.5+math.random() }
        self.moveSpeed = math.random()
        self.doodad = Server.Doodad.Spawn("Test")
        --Server.Message.Later(7, self.id, "Suicide")
        if self.id < 50 then
            Server.Message.Later(3, self.id, "Test")
        end
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

    Suicide = function(self)
        Server.Entity.Kill(self.id)
    end,

    Move = function(self)
        local pos = Server.Entity.GetPos(self.id)
        Server.Entity.SetPos(self.id, { x = pos.x + self.move.x / 8, y = pos.y + self.move.y / 8, z = pos.z + self.move.z / 8 })
        Server.Message.Later(self.moveSpeed, self.id, "Move")
    end,

    Test = function(self)
        Server.Entity.Spawn(Server.Entity.GetPos(self.id), "Blob")
    end

}
