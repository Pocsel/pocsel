-- Server.Entity.Register{
--     name = "CubeDeleter",
--     Spawn = function(entity, this)
--         Server.Event.CallLater(1, this.id, "_DeleteCube", { pos = Server.Pos.New() })
--     end,
--     _DeleteCube = function(entity, this, args)
--         Server.Cube.Delete(args.pos)
--         Server.Event.CallLater(1, this.id, "_DeleteCube", { pos = Server.Pos.Add(args.pos, 1, 0, 0) })
--     end,
-- }

Server.Entity.Register{
    name = "GrosTest",
    Spawn = function(entity, this)
        Server.Event.CallLater(2, this.id, "_TestMethod", { testArg = "ceci est un test" })
    end,
    _TestMethod = function(entity, this, args)
        print("Entity " .. this.id .. " ('" .. entity.name .. "'): " .. args.testArg)
        Server.Event.CallLater(20, this.id, "_TestMethod", { testArg = args.testArg .. "!" })
    end,
}
