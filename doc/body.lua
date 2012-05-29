-- Server

Server.Entity.RegisterPositional{
    entityName = "Tank",
    Spawn = function(self)
        self.d = Server.Doodad.Spawn("Tank", "Tank")
    end,
    DoStuff = function(self)
        Server.Entity.SetPos(self.id, { X, Y, Z }) -- bouge tous les bodies
        Server.Entity.SetAngle(self.id, { A, B, C }) -- tourne tous les bodies
        Server.Body.SetPos(self.d, "Cannon", { X, Y, Z }) -- bouge le cannon par rapport a l'origine de son noeud
        Server.Body.SetScale(self.d, "Cannon", { X, Y, Z }) -- bouge le cannon par rapport a l'origine de son noeud
    end,
}

-- plus tard
-- Server.Doodad.Register{
--     doodadName = "Tank",
--     body = "Tank",
-- }

Server.Body.Register{
    bodyName = "Tank",
    physics = {
        -- friction, bounciness, etc
        -- peut etre que ca sera par noeud ou shape
    },
    shapeTree = {
        {
            name = "Base", -- nom unique de noeud
            offset = { X, Y, Z }, -- par rapport au parent (entité positionnelle)
            angle = { A, B, C }, -- idem
            boxes = {
                {
                    -- par rapport au noeud
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
        self.cannon = Client.Model.Spawn("TankCannon", "Cannon")
        self.turret = Client.Model.Spawn("TankTurret", "Turret")
        self.base = Client.Model.Spawn("TankBase", "Base")
    end,
}

Client.Model.Register{
    modelName = "Tank",
    angles = {
        Turret = {

        }
    }
}
