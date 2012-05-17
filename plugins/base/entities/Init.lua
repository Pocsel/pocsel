Server.Entity.Register{

    entityName = "Init", -- mandatory

    Spawn = function(self) -- constructor
        print("Spawn()")
        self.i = 0
        self:Loop()
        for i = 0, 3 do
            Srv.Entity.Spawn("Blob", self.id)
        end
    end,

    Die = function(self) -- destructor
        print("Die()")
    end,

    Save = function(self) -- save to database
        print("Save()")
        self.storage.i = self.i
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

Server.Entity.Register{

    entityName = "Blob",

    Spawn = function(self, initEntity)
        self:Loop(initEntity)
    end,

    Loop = function(self, initEntity)
        Srv.Message.Later(5 + math.random() * 20, initEntity, "GetCounter", nil, self.id, "GotCounter")
    end,

    GotCounter = function(self, arg, result)
        print(result.ret)
        self:Loop(result.entityId)
    end,

}
