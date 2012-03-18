Server.Entity.Register{
    entityName = "Init",
    Spawn = function(self)
        self:Method()
        Server.Call.Later(5, self.id, "EventTest", 1)
    end,
    Method = function(self)
        print("hey from " .. self.entityName)
        print("prototype " .. self.prototype.entityName, self.prototype.id)
    end,
    EventTest = function(self, nbCalls)
        print(nbCalls)
        Server.Call.Later(5, self.id, "EventTest", nbCalls, self.id, "Notify", "Test!")
    end,
    Notify = function(self, arg, result)
        print(arg, result.entityId, result.success)
        Server.Call.Later(5, 12, "ca va pas marcher", nil, self.id, "NotifyError")
    end,
    NotifyError = function(self, arg, r)
        print("NotifyError: " .. tostring(arg) .. ", " .. r.entityId .. ", " .. tostring(r.success))
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
--        entity.maxiTable = {}
--
--        entity.storage.maxiTable = entity.maxiTable
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
--
--
--Server.Entity.Register{
--    name = "Bite",
--    Spawn = function(self)
--        self:_Method()
--        self.type.name
--        self.bite = 12
--    end,
--    _Method = function(self)
--    end,
--}
--
--Server.Entity.Register{
--    name = "Bite",
--    Spawn = function()
--        self.type._Method()
--    end,
--    _Method = function()
--    end,
--}
--
--function Account:Deposit(amount)
--    self.balance = self.balance + amount
--end
