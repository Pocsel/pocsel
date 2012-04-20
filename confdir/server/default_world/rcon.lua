-- List of users having access to the remote console. An empty or nil password
-- disables the access for a particular user.
--
-- Available rights:
--  - rcon_sessions: viewing other connected rcon users
--  - logs: viewing logs
--  - entities: viewing entity instances
--  - messages: viewing messages exchanged between entities
--  - execute: executing Lua code on any map & editing entity files
--
-- With no rights, a user with a password has still access to the list of maps,
-- plugins, entity types, players and entity files (without edit).
rconUsers = {
    paps = {
        password = "paps",
        rights = { "rcon_sessions", "logs", "entities", "messages", "execute" }
    },
    plug = {
        password = "paps",
        rights = { "rcon_sessions" }
    },
    dre = {
        password = "paps",
        rights = { "rcon_sessions" }
    },
}
