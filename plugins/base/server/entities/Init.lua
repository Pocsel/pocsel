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
        Server.Message.Later(22, self.id, "SpawnBlob")
        Server.Message.Later(17, self.id, "SpawnTank")
        Server.Entity.Spawn("base:TestWeakPtr")
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
        Server.Entity.Spawn(Utils.Vector3(67108864, 16777216 + 23, 67108864), "base:Ball")
        -- Server.Message.Later(6, self.id, "SpawnBlob")
    end,

    SpawnTank = function(self)
        Server.Entity.Spawn(Utils.Vector3(67108864, 16777216 + 33, 67108864), "base:Tank") -- , "base:Tank")
        if not self.tankNumber then
            self.tankNumber = 0
        end
        self.tankNumber = self.tankNumber + 1
        if self.tankNumber < 10 then
            --Server.Message.Later(11, self.id, "SpawnTank")
        end
    end,

    CallMe = function(self, continue)
        if not self.toto then
            self.toto = 20
        else
            self.toto = self.toto + 2
        end
        if continue then
            self:CallMe(self.toto < 30)
        end
    end,
}

Server.Entity.RegisterPositional{

    entityName = "Blob",

    Spawn = function(self)
        self.moveSpeed = 10 -- 3 + math.random() * 3

        self.doodad = Server.Doodad.Spawn(self.id, "base:Test", "base:Test")
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
        --[[--
        {
            name = "NoeudDeTestALaCon", -- nom unique de noeud
            position = { 2, 0, 0 }, -- par rapport au parent (entité positionnelle)
            orientation = { 0, 0, 0 }, -- idem, yawpitchroll
            shape = {
                shapeType = "box", -- pour le moment y'a que box ou sphere
                halfExtents = {0.8, 1.9, 1.2}
            },
            friction = 0.5,
            restitution = 0.0,
            mass = 70,
            children = {
                {
                    name = "nique0",
                    position = { 5, 0, 0 }, -- par rapport au parent (entité positionnelle)
                    orientation = { 0, 0, 0 }, -- idem, yawpitchroll
                    shape = {
                        shapeType = "box", -- pour le moment y'a que box ou sphere
                        halfExtents = {0.8, 1.9, 1.2}
                    },
                    friction = 0.5,
                    restitution = 0.0,
                    mass = 70,
                },
                {
                    name = "nique1",
                    position = { 0, 5, 0 }, -- par rapport au parent (entité positionnelle)
                    orientation = { 0, 0, 0 }, -- idem, yawpitchroll
                    shape = {
                        shapeType = "box", -- pour le moment y'a que box ou sphere
                        halfExtents = {0.8, 1.9, 1.2}
                    },
                    friction = 0.5,
                    restitution = 0.0,
                    mass = 70,
                },
                {
                    name = "nique2",
                    position = { 0, 0, 5 }, -- par rapport au parent (entité positionnelle)
                    orientation = { 0, 0, 0 }, -- idem, yawpitchroll
                    shape = {
                        shapeType = "box", -- pour le moment y'a que box ou sphere
                        halfExtents = {0.8, 1.9, 1.2}
                    },
                    friction = 0.5,
                    restitution = 0.0,
                    mass = 70,
                },
                {
                    name = "nique_0",
                    position = { -5, 0, 0 }, -- par rapport au parent (entité positionnelle)
                    orientation = { 0, 0, 0 }, -- idem, yawpitchroll
                    shape = {
                        shapeType = "box", -- pour le moment y'a que box ou sphere
                        halfExtents = {0.8, 1.9, 1.2}
                    },
                    friction = 0.5,
                    restitution = 0.0,
                    mass = 70,
                },
                {
                    name = "nique_1",
                    position = { 0, -5, 0 }, -- par rapport au parent (entité positionnelle)
                    orientation = { 0, 0, 0 }, -- idem, yawpitchroll
                    shape = {
                        shapeType = "box", -- pour le moment y'a que box ou sphere
                        halfExtents = {0.8, 1.9, 1.2}
                    },
                    friction = 0.5,
                    restitution = 0.0,
                    mass = 70,
                },
                {
                    name = "nique_2",
                    position = { 0, 0, -5 }, -- par rapport au parent (entité positionnelle)
                    orientation = { 0, 0, 0 }, -- idem, yawpitchroll
                    shape = {
                        shapeType = "box", -- pour le moment y'a que box ou sphere
                        halfExtents = {0.8, 1.9, 1.2}
                    },
                    friction = 0.5,
                    restitution = 0.0,
                    mass = 70,
                },
            }
        },
        --]]--
        ---[[--
        {
            name = "NoeudDeTestALaCon", -- nom unique de noeud
            position = { 0, 0, 0 }, -- par rapport au parent (entité positionnelle)
            orientation = { 0, 0, 0 }, -- idem, yawpitchroll
            shape = {
                shapeType = "box", -- pour le moment y'a que box ou sphere
                halfExtents = {1.1, 1.1, 1.1},
            },
            friction = 0.5,
            restitution = 0.0,
            mass = 70,
            children = {
        {
            name = "NoeudDeTestALaCon000", -- nom unique de noeud
            position = { -3, -3, -3 }, -- par rapport au parent (entité positionnelle)
            orientation = { 0, 0, 0 }, -- idem, yawpitchroll
            shape = {
                shapeType = "box", -- pour le moment y'a que box ou sphere
                halfExtents = {0.8, 1.9, 1.2}
            },
            friction = 0.5,
            restitution = 0.0,
            mass = 70,
        },
        {
            name = "NoeudDeTestALaCon001", -- nom unique de noeud
            position = { -3, -3, 3 }, -- par rapport au parent (entité positionnelle)
            orientation = { 0, 0, 0 }, -- idem, yawpitchroll
            shape = {
                shapeType = "box", -- pour le moment y'a que box ou sphere
                halfExtents = {0.8, 1.9, 1.2}
            },
            friction = 0.5,
            restitution = 0.0,
            mass = 70,
        },
        {
            name = "NoeudDeTestALaCon010", -- nom unique de noeud
            position = { -3, 3, -3 }, -- par rapport au parent (entité positionnelle)
            orientation = { 0, 0, 0 }, -- idem, yawpitchroll
            shape = {
                shapeType = "box", -- pour le moment y'a que box ou sphere
                halfExtents = {0.8, 1.9, 1.2}
            },
            friction = 0.5,
            restitution = 0.0,
            mass = 70,
        },
        {
            name = "NoeudDeTestALaCon011", -- nom unique de noeud
            position = { -3, 3, 3 }, -- par rapport au parent (entité positionnelle)
            orientation = { 0, 0, 0 }, -- idem, yawpitchroll
            shape = {
                shapeType = "box", -- pour le moment y'a que box ou sphere
                halfExtents = {0.8, 1.9, 1.2}
            },
            friction = 0.5,
            restitution = 0.0,
            mass = 70,
        },
        {
            name = "NoeudDeTestALaCon100", -- nom unique de noeud
            position = { 3, -3, -3 }, -- par rapport au parent (entité positionnelle)
            orientation = { 0, 0, 0 }, -- idem, yawpitchroll
            shape = {
                shapeType = "box", -- pour le moment y'a que box ou sphere
                halfExtents = {0.8, 1.9, 1.2}
            },
            friction = 0.5,
            restitution = 0.0,
            mass = 70,
        },
        {
            name = "NoeudDeTestALaCon101", -- nom unique de noeud
            position = { 3, -3, 3 }, -- par rapport au parent (entité positionnelle)
            orientation = { 0, 0, 0 }, -- idem, yawpitchroll
            shape = {
                shapeType = "box", -- pour le moment y'a que box ou sphere
                halfExtents = {0.8, 1.9, 1.2}
            },
            friction = 0.5,
            restitution = 0.0,
            mass = 70,
        },
        {
            name = "NoeudDeTestALaCon110", -- nom unique de noeud
            position = { 3, 3, -3 }, -- par rapport au parent (entité positionnelle)
            orientation = { 0, 0, 0 }, -- idem, yawpitchroll
            shape = {
                shapeType = "box", -- pour le moment y'a que box ou sphere
                halfExtents = {0.8, 1.9, 1.2}
            },
            friction = 0.5,
            restitution = 0.0,
            mass = 70,
        },
        {
            name = "NoeudDeTestALaCon111", -- nom unique de noeud
            position = { 3, 3, 3 }, -- par rapport au parent (entité positionnelle)
            orientation = { 0, 0, 0 }, -- idem, yawpitchroll
            shape = {
                shapeType = "box", -- pour le moment y'a que box ou sphere
                halfExtents = {0.8, 1.9, 1.2}
            },
            friction = 0.5,
            restitution = 0.0,
            mass = 70,
        },
        }
        }
        --]]--
--        {
--            ... -- autre noeud a la racine
--        },
    }
}
