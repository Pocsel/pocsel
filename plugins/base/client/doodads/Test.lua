Client.Doodad.Register{

    doodadName = "Test",

    Spawn = function(self)
        print("Spawn()")
        Client.Model.Spawn("Stickman")
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

}
