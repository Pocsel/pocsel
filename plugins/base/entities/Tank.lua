Server.Entity.RegisterPositional{
    entityName = "Tank",

    Spawn = function(self)
        self.d = Server.Doodad.Spawn(self.id, "Tank", "Tank")
    end,

    DoStuff = function(self)
        Server.Entity.SetPos(self.id, { X, Y, Z }) -- bouge tous les bodies
        Server.Entity.SetAngle(self.id, { A, B, C }) -- tourne tous les bodies
        Server.Entity.SetScale(self.id, {1, 2, 1})

        Server.Body.SetPos(self.d, "Cannon", { X, Y, Z }) -- bouge le cannon par rapport a l'origine de son noeud
        Server.Body.SetAngle(self.d, "Cannon", { X, Y, Z }) -- bouge le cannon par rapport a l'origine de son noeud
        Server.Body.SetScale(self.d, "Cannon", { X, Y, Z }) -- bouge le cannon par rapport a l'origine de son noeud

        Server.Doodad.Set(self.d, "variable", "valeur")
        Server.Doodad.Call(self.d, "Function", "test")
        Server.Doodad.SetUdp(self.d, "variable", "valeur")
        Server.Doodad.CallUdp(self.d, "Function", "test")
    end,
}

Server.Body.Register{
    bodyName = "Tank",
    physics = {
        -- friction, bounciness, etc
        -- peut etre que ca sera par noeud ou pshape
    },
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
