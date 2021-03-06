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
            print("Hull & move canon forward")
            --Server.Entity.SetAccel(self.id, Utils.Vector3(0, 0, 0), 10)

            d = self.doodadPtr:Lock()
            if d then
                --Server.Doodad.SetLocalAccel(self.doodadPtr, "Hull", Utils.Vector3(500, 0, 0), 10)
                --Server.Doodad.SetInterPositionTarget(self.doodadPtr, "Gun", Utils.Vector3(10, 0, 0), 10)
                Server.Doodad.SetInterAngleTarget(self.doodadPtr, "Turret", Utils.Vector3(0, 0, 0), 1.5)
                Server.Doodad.SetInterAngleTarget(self.doodadPtr, "Gun", Utils.Vector3(0, 0, 0), 1.5)
                --d:SetInterAngleTarget("Turret", Utils.Vector3(0, 2.3, 0), 10) -- roll yaw pitch ??
                -- d:SetInterAngleTarget("Gun", Utils.Vector3(0, 0, 0.8), 10) -- roll yaw pitch ??
            end

            -- tmpPos.y = tmpPos.y + 10
            self.upupup = false
        else
            print("errything & rotate le toit")
            --Server.Entity.SetAccel(self.id, Utils.Vector3(0, 13, 0), 10)

            d = self.doodadPtr:Lock()
            if d then
                Server.Doodad.SetInterAngleTarget(self.doodadPtr, "Turret", Utils.Vector3(1, 2, 3), 1)
                Server.Doodad.SetInterAngleTarget(self.doodadPtr, "Gun", Utils.Vector3(2, 2, 2), 2)
                -- Server.Doodad.SetInterAngleLimits(self.doodadPtr, "Gun", Utils.Vector3(0, 0, 1))
                --   Server.Doodad.SetInterAngleLimits(self.doodadPtr, "Gun", Utils.Vector3(1, 0, 0))
                --d:SetAccel("Hull", Utils.Vector3(0, 0, 0), 10)
                --Server.Doodad.SetInterPositionTarget(self.doodadPtr, "Turret", Utils.Vector3(0, 10, 0), 1)
                --d:SetInterAngleTarget("Turret", Utils.Vector3(0, 0, 1.5), 10)
                --Server.Doodad.SetInterPositionTarget(self.doodadPtr, "Gun", Utils.Vector3(10, 0, 0), 10)
                --Server.Doodad.SetInterPositionTarget(self.doodadPtr, "Turret", Utils.Vector3(10, 4, 1), 5)
                --Server.Doodad.SetInterAngleTarget(self.doodadPtr, "Turret", Utils.Vector3(0, 10, 0), 10)
                --d:SetInterAngleTarget("Gun", Utils.Vector3(0, 0, -3.1), 10) -- roll yaw pitch ??
            end

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
            position = { 0, 1, 0 }, -- par rapport au parent (entité positionnelle)
            orientation = { 0, 0, 0 }, -- idem, yawpitchroll
            shape = {
                --        shapeType = "sphere",
                --        radius = 4
                 shapeType = "box", -- pour le moment y'a que box ou sphere
                 halfExtents = {5.51, 1.01, 4.01}
            },
            mass = 1200,

            ---[[--
            children = {
                {
                    name = "Turret",
                    position = { 0, 1, 0 }, -- par rapport au parent (noeud Hull)
                    orientation = { 0, 0, 0 }, -- idem, yawpitchroll
                    shape = {
                        shapeType = "sphere",
                        radius = 2.01
                    },
                    mass = 300,

                    children = {
                        {
                            name = "Gun",
                            position = { 2.66, 10.75, 0 }, -- par rapport au parent (noeud Turret)
                            orientation = { 0, 0, -3.14/2 }, -- idem, yawpitchroll
                            --orientation = { 3.14, -3.14, 3.14/2 }, -- idem, yawpitchroll
                            shape = {
                                shapeType = "box",
                                halfExtents = {0.5, 3, 1.2}
                                --halfExtents = {3, 1, 2}

                                --halfExtents = {3.34, 0.51, 0.51}
                                --halfExtents = {0.51, 3.34, 0.51}
                            },
                            friction = 0.01,
                            restitution = 0.99,
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
