Client.Doodad.Register{

    doodadName = "TestDoodad",

    Spawn = function(self)
        print("Spawn()")
        self.m = Client.Model.Spawn("Stickman")
    end,

    Think = function(self)
        -- bourrinage
    end,

    Die = function(self)
        print("Die()")
    end,

    TestFunction = function(self)
        print("Heyyyy!! :)")
    end,

    PhysicsUpdateCallback = function(self)
        print("HEYOYOYOYOOYOY")
    end,

}
