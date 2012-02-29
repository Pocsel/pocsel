Server.Entity.Register{
    name = "CubeDeleter",
    Spawn = function(entity, this)
        Server.Event.CallLater(1, this.id, "DeleteCube", { pos = Server.Pos.New() })
    end,
    DeleteCube = function(entity, this, args)
        Server.Cube.Delete(args.pos)
        Server.Event.CallLater(1, this.id, "DeleteCube", { pos = Server.Pos.Add(args.pos, 1, 0, 0) })
    end
}
