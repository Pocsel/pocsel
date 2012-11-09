Server.Entity.RegisterPositional{
    entityName = "Tank",

    Spawn = function(self)
        self.doodadPtr = Server.Doodad.Spawn(self.id, "base:Tank", "base:Tank")
        Server.Message.Later(5, self.id, "DoShit")
        self.upupup = true
    end,

    DoShit = function(self)
        tmpPos = Server.Entity.GetPos(self.id)
        if self.upupup then
            print("LOCAL")
            Server.Entity.SetLocalAccel(self.id, Utils.Vector3(20, 0, 0), 10)
            -- tmpPos.y = tmpPos.y + 10
            self.upupup = false
        else
            print("NONLOCAL")
            Server.Entity.SetAccel(self.id, Utils.Vector3(0, 20, -10), 10)
            -- tmpPos.y = tmpPos.y - 10
            self.upupup = true
        end
        -- Server.Entity.SetPos(self.id, tmpPos)
        -- Server.Entity.SetPos(self.id, Utils.Vector3(67108864, 16777216 + 10, 67108864))
        Server.Message.Later(10, self.id, "DoShit")
    end,

    DoStuff = function(self)
        Server.Entity.SetPos(self.id, { X, Y, Z }) -- bouge tous les bodies
        Server.Entity.SetAngle(self.id, { A, B, C }) -- tourne tous les bodies

        --Server.Body.SetPos(self.d, "Cannon", { X, Y, Z }) -- bouge le cannon par rapport a l'origine de son noeud
        --Server.Body.SetAngle(self.d, "Cannon", { X, Y, Z }) -- bouge le cannon par rapport a l'origine de son noeud
        --Server.Body.SetScale(self.d, "Cannon", { X, Y, Z }) -- bouge le cannon par rapport a l'origine de son noeud

        d = self.doodadPtr:Lock()
        if d ~= nil then
            d:Set("variable", "valeur")
            d:Call("Function", "test")
            d:SetUdp( "variable", "valeur")
            d:CallUdp("Function", "test")
        end
    end,
}

Server.Body.Register{
    bodyName = "Tank",
    physics = {
        friction = 0.5,
        restitution = 0.0
        -- valeurs par defaut, peuvent etre changees a chaque noeud
    },
    shapeTree = {
        {
            name = "Hull", -- nom unique de noeud
            position = { 0, 0, 0 }, -- par rapport au parent (entité positionnelle)
            orientation = { 0, 0, 0 }, -- idem, yawpitchroll
            shape = {
                --        shapeType = "sphere",
                --        radius = 4
                 shapeType = "box", -- pour le moment y'a que box ou sphere
                 halfExtents = {4, 2, 4}
            },
            mass = 1200,

            ---[[--
            children = {
                {
                    name = "Turret",
                    position = { 0, 4, 0 }, -- par rapport au parent (noeud Hull)
                    orientation = { 0, 0, 0 }, -- idem, yawpitchroll
                    shape = {
                        shapeType = "sphere",
                        radius = 2
                    },
                    mass = 300,

                    children = {
                        {
                            name = "Gun",
                            position = { 3, 0, 0 }, -- par rapport au parent (noeud Turret)
                            orientation = { 0, 0, 0 }, -- idem, yawpitchroll
                            shape = {
                                shapeType = "box",
                                halfExtents = {3, 0.2, 0.2}
                            },
                            friction = 0.01,
                            restitution = 9.99,
                            mass = 100,
                        },
                    }
                },
            },
            --]]--
        }
--        {
--            ... -- autre noeud a la racine
--        },
    }
}
