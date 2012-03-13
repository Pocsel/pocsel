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

--Server.Entity.Register{
--    name = "Init",
--
--    Spawn = function(entity, this, args, spawnerId)
--        -- entity n'est pas sauve a la fermeture du serveur
--        print("Entity Init du plugin base.")
--        S.Event.CallLater(2, this.id, "_EventTest", { biteBite = "sdfgsdfg" })
--        S.Event.SelfCall("_EventTest", { sdfg = 12 })
--        entity._EventTest(entity, this, {}, this.id)
--        S.Event.SelfCallLater(60, "_EventTest", { sdgffff = 14 })
--    end,
--
--    Save = function(entity, this)
--        this.storage.preciousSavedItem = this.itemId
--    end,
--
--    Load = function(entity, this)
--        this.itemId = this.storage.preciousSavedItem
--    end,
--
--    Die = function(entity, this, args, killerId)
--        print("No!!")
--    end,
--
--    _EventTest = function(entity, this, args, callerId)
--        print(args.biteBite);
--        S.Event.CallLater(2, this.id, "_EventTest", { biteBite = args.biteBite .. "TEST" })
--        S.Event.Call(this.id, "_EventTest", { arg1 = "sdgsdg" })
--        S.Entity.Spawn("Init", { test = "test" }, this.id, "_InitRespawned", { info = 6.44 })
--        S.Entity.SpawnFromPlugin("Dog", "dog-plugin", { arg1 = "!!" }, this.id, "_DogSpawned", { bite = "8==D" })
--        S.Entity.Kill(this.id, { reason = "none :p" }, this.id, "_KillCallback", { hey = -12 })
--    end,
--
--    _InitRespawned = function(entity, this, args, spawnedId)
--    end,
--
--    _KillCallback = function(entity, this, args)
--    end,
--}
