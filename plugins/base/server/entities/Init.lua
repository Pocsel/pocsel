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
        Server.Message.Later(12, self.id, "SpawnBlob")
        Server.Message.Later(18, self.id, "SpawnTank")
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
        Server.Message.Later(6, self.id, "SpawnBlob")
    end,

    SpawnTank = function(self)
        Server.Entity.Spawn(Utils.Vector3(67108864, 16777216 + 3, 67108864), "base:Tank") -- , "base:Tank")
        Server.Message.Later(11, self.id, "SpawnTank")
    end,
}

Server.Entity.RegisterPositional{

    entityName = "Blob",

    Spawn = function(self)
        self.moveSpeed = 10 -- 3 + math.random() * 3

        self.doodad = Server.Doodad.Spawn("base:Test", "base:Test")
--        Server.Message.Later(30, self.id, "Suicide")
--         if self.id < 50 then
--             Server.Message.Later(4, self.id, "Test")
--         end
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

Server.Body.Register{
    bodyName = "Test",
    physics = {
        friction = 0.5,
        restitution = 0.0
        -- valeurs par defaut, peuvent etre changees a chaque noeud
    },
    shapeTree = {
        {
            name = "NoeudDeTestALaCon", -- nom unique de noeud
            position = { 0, 0, 0 }, -- par rapport au parent (entité positionnelle)
            orientation = { 0, 0, 0 }, -- idem, yawpitchroll
            shape = {
                shapeType = "box", -- pour le moment y'a que box ou sphere
                halfExtents = {0.8, 1.9, 1.2}
            },
            friction = 0.5,
            restitution = 0.0,
            mass = 70,
        }
--        {
--            ... -- autre noeud a la racine
--        },
    }
}
