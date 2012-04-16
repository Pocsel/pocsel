-- List of users having access to the remote console. An empty password or nil
-- disables the access for a particular user.
--
-- Available rights:
--  - rcon_sessions: viewing other connected rcon users
--
-- With no rights, a user with a password has still access to the list of maps,
-- plugins, entity types, players and entity files (without edit).
rconUsers = {
    paps = {
        password = "paps",
        rights = { "rcon_sessions" }
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
