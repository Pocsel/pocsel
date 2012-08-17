Server.Entity.Register{

    entityName = "TestWeakPtr",

    Spawn = function(self)
        self.totos = {}
        for i = 0, 4 do
            Server.Entity.Spawn("base:Toto", nil, self.id, "TotoSpawned")
        end
        Server.Message.Later(10, self.id, "CallTotos")
    end,

    Save = function(self)
        Server.Entity.Save(self.id)
        return false
    end,

    Load = function(self)
        Server.Entity.Load(self.id)
    end,

    TotoSpawned = function(self, arg, result)
        table.insert(self.totos, Server.Entity.GetEntityById(result.entityId))
    end,

    CallTotos = function(self)
        for k, ptr in pairs(self.totos) do
            local e = ptr:Lock()
            if e then
                e:TotoTest()
            end
        end
        Server.Message.Later(10, self.id, "CallTotos")
    end,

}

Server.Entity.Register{

    entityName = "Toto",

    TotoTest = function(self)
        print("Je suis Toto avec l'id ", self.id, " !")
    end,

}
