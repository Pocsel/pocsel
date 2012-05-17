Server.CubeType.Register{
	name = "dirt",
	transparent = false,
	solid = true,
	visualEffect = Server.Resource.GetEffect("cubes/dirt/visual.lua"),
	rotation = 1 + 2 + 4,
}

Server.CubeType.Register{
	name = "grass",
	transparent = false,
	solid = true,
	visualEffect = Server.Resource.GetEffect("cubes/grass/visual.lua"),
	rotation = 0,
}

Server.CubeType.Register{
	name = "iron",
	transparent = false,
	solid = true,
	visualEffect = Server.Resource.GetEffect("cubes/iron/visual.lua"),
	rotation = 0,
}

Server.CubeType.Register{
	name = "water",
	transparent = true,
	solid = false,
	visualEffect = Server.Resource.GetEffect("cubes/water/visual.lua"),
	rotation = 0,
}

Server.CubeType.Register{
	name = "rock",
	transparent = false,
	solid = true,
	visualEffect = Server.Resource.GetEffect("cubes/rock/visual.lua"),
	rotation = 0,
}
