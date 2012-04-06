Server.CubeType.Register{
        name = "dirt",
        transparent = false,
        solid = true,
        visualEffect = Server.Resource.GetEffect("cubes/dirt/visual.lua"),
        rotation = 1 + 2 + 4,
        bite = function (toto)
            Server.Resource.GetEffect("cubes/dirt/visual.lua")
        end
    }
