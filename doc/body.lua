-- Server

Server.Entity.RegisterPositional{
    entityName = "Tank",
    Spawn = function(self)
        self.d = Server.Doodad.Spawn(self.id, "Tank", "Tank")
    end,
    DoStuff = function(self)
        Server.Entity.SetPos(self.id, { X, Y, Z }) -- bouge tous les bodies
        Server.Entity.SetAngle(self.id, { A, B, C }) -- tourne tous les bodies
        Server.Body.SetPos(self.d, "Cannon", { X, Y, Z }) -- bouge le cannon par rapport a l'origine de son noeud
        Server.Body.SetScale(self.d, "Cannon", { X, Y, Z }) -- bouge le cannon par rapport a l'origine de son noeud
        Server.Doodad.Set(self.d, "variable", "valeur")
        Server.Doodad.Call(self.d, "Function", "test")
        Server.Doodad.SetUdp(self.d, "variable", "valeur")
        Server.Doodad.CallUdp(self.d, "Function", "test")
    end,
}

Server.Entity.RegisterPositional{
    entityName = "Player",
    Spawn = function(self, playerId)
        Server.PlayerScript.Bind(playerId, "UpdateAngle", self.id, "UpdateAngle")
        Server.PlayerScript.OnQuit(playerId, self.id, "PlayerQuit")
        self.doodad = Server.Doodad.Spawn(self.id, "Player", "Player")
        Server.Doodad.DisableForPlayer(self.doodad, playerId)
    end,
    UpdateAngle = function(self)
    end,
    PlayerQuit = function(self)
        Server.Entity.Kill(self.id)
    end,
}

-- plus tard
-- Server.Doodad.Register{
--     doodadName = "Tank",
--     body = "Tank",
--     fastsyncstuff = {
--         toto = "double",
--         blabla = "vector3d",
--         truc
--     }
-- }

Server.Body.Register{
    bodyName = "Tank",
    physics = {
        -- friction, bounciness, etc
        -- peut etre que ca sera par noeud ou pshape
    },                                        S
    shapeTree = {                              
        {                                      
            name = "Base", -- nom unique de no eud
            offset = { X, Y, Z }, -- par rappo rt au parent (entité positionnelle)
            angle = { A, B, C }, -- idem       
            boxes = {                          
                {                              
                    -- par rapport au noeud   }
                    { X, Y, Z },
                    { W, H, L },
                    { A, B, C },
                },
            },
            spheres = {
                {
                    -- par rapport au noeud
                    { X, Y, Z },
                    R
                },
            },
            children = {
                {
                    name = "Turret",
                    offset = { X, Y, Z }, -- par rapport au parent (noeud Base)
                    angle = { A, B, C }, -- idem
                    boxes = {
                        {
                            { X, Y, Z },
                            { W, H, L },
                            { A, B, C },
                        },
                    },
                    children = {
                        {
                            name = "Cannon",
                            offset = { X, Y, Z },
                            angle = { A, B, C },
                            boxes = {
                                {
                                    { X, Y, Z },
                                    { W, H, L },
                                    { A, B, C },
                                },
                            },
                        },
                    }
                },
                {
                    ...
                },
            },
        }
        {
            ... -- autre noeud a la racine
        },
    }
}

-- Client

Client.Doodad.Register{
    doodadName = "Tank",
    Spawn = function(self)
        self.model = Client.Model.Spawn(self.id, "Tank")
    end,
}

Client.Model.Register{
    modelName = "Tank",
    angles = {
        Turret = {
            turret = 1
        },
        Cannon = {
            cannon = 1
        }
    }
}

Client.PlayerScript.Register{
}

Client.Doodad.Register{
    doodadName = "Player",
    Spawn = function(self)
        self.model = Client.Model.Spawn(self.id, "Player")
    end,
}

Client.Model.Register{
    modelName = "Player",
    angles = {
        ,
    }
}
