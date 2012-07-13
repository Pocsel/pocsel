local SampleModule = require "base:entities/SampleModule.lua"

Server.Entity.Register{

    entityName = "Init", -- mandatory

    Spawn = function(self) -- constructor
        print("Spawn()")
        if SampleModule ~= nil then
            print("Test Module: ", SampleModule.testVal)
        else
            print("Test Module nil")
        end
        Server.Message.Later(20, self.id, "SpawnBlob")
        Server.Message.Later(25, self.id, "SpawnTank")
    end,

    Die = function(self) -- destructor
        print("Die()")
    end,

    Save = function(self, chunkUnloaded) -- save to database
        print("Save() ", tostring(chunkUnloaded))
        Server.Entity.Save(self.id)
        return false
    end,

    Load = function(self) -- load from database
        print("Load()")
        Server.Entity.Load(self.id)
    end,

    SpawnBlob = function(self)
        Server.Entity.Spawn(Utils.Vector3(67108864, 16777216 + 10, 67108864), "base:Blob")
        Server.Message.Later(3, self.id, "SpawnBlob")
    end,

    SpawnTank = function(self)
        Server.Entity.Spawn(Utils.Vector3(67108864, 16777216 + 12, 67108864), "base:Tank") -- , "base:Tank")
    end,
}

Server.Entity.RegisterPositional{

    entityName = "Blob",

    Spawn = function(self)
        self.moveSpeed = 10 -- 3 + math.random() * 3

        self.doodad = Server.Doodad.Spawn("base:Test")
--        Server.Message.Later(30, self.id, "Suicide")
        if self.id < 50 then
            Server.Message.Later(4, self.id, "Test")
        end
        self:Move()
    end,

    Die = function(self)
        print("Die()");
    end,

    Save = function(self, chunkUnloaded)
        print("Save() ", tostring(chunkUnloaded))
        Server.Entity.Save()
        return false
    end,

    Load = function(self)
        print("Load()")
        Server.Entity.Load()
    end,

    Suicide = function(self)
        Server.Entity.Kill(self.id)
    end,

    Move = function(self)
        -- local pos = Server.Entity.GetPos(self.id)
        -- Server.Entity.SetSpeed(self.id, Utils.Vector3(-0.5+math.random(), -0.5+math.random(), -0.5+math.random())) -- { x = pos.x + self.move.x / 8, y = pos.y + self.move.y / 8, z = pos.z + self.move.z / 8 })
        Server.Entity.SetAccel(self.id, Utils.Vector3(-0.5+math.random(), -0.5+math.random(), -0.5+math.random())) -- { x = pos.x + self.move.x / 8, y = pos.y + self.move.y / 8, z = pos.z + self.move.z / 8 })
        Server.Message.Later(self.moveSpeed, self.id, "Move")
    end,

    Test = function(self)
        Server.Entity.Spawn(Server.Entity.GetPos(self.id), "base:Blob")
    end

}
