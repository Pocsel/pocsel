Server.Entity.Register{
    name = "Init",
    Spawn = function(entity, this)
        print("Entity Init du plugin base.")
        S.Event.CallLater(2, this.id, "_EventTest", { biteBite = "sdfgsdfg" })
    end,
    _EventTest = function(entity, this, args)
        print(args.biteBite);
        S.Event.CallLater(2, this.id, "_EventTest", { biteBite = args.biteBite .. "TEST" })
    end,
}
