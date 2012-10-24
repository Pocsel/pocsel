Server.Entity.RegisterPositional{

    entityName = "Ball",

    Spawn = function(self)
        self.doodad = Server.Doodad.Spawn(self.id, "base:Ball", "base:Ball")
        toto = Server.Doodad.GetWeakPointer(self.doodad)
        d = toto:Lock()
        if d then
            d:Set("caca", 12)
        end
    end,

}

Server.Body.Register{

    bodyName = "Ball",

    physics = {
        friction = 1,
        restitution = 0.9
    },

    shapeTree = {
        {
            name = "Ball",
            position = { 0, 0, 0 },
            orientation = { 0, 0, 0 },
            shape = {
                shapeType = "sphere",
                radius = 0.5
            },
            mass = 10
        }
    }
}
