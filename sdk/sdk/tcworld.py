# -*- encoding: utf-8 -*-

from sdk import tcplug

def insertResource(worldconn, pluginconn, plugin_id, filename, version):
    print "Insert new resource '%s'" % filename
    with pluginconn() as pconn:
        pcurs = pconn.cursor()
        pcurs.execute(
            "SELECT type, data_hash FROM resource WHERE filename = ?",
            (filename,)
        )
        res = pcurs.fetchone()
        assert res is not None
        type = res[0]
        data_hash = res[1]

    with worldconn() as wconn:
        wcurs = wconn.cursor()
        wcurs.execute(
            """
                INSERT INTO resource
                    (id, plugin_id, version, filename, data_hash, type, data)
                VALUES (NULL, ?, ?, ?, ?, ?, NULL)
            """,
            (plugin_id, version, filename, data_hash, type)
        )
        resource_id = wcurs.lastrowid
        wconn.commit()

    with pluginconn() as pconn:
        pconn.text_factory = str
        pcurs = pconn.cursor()
        pcurs.execute("SELECT data FROM resource WHERE filename = ?", (filename,))
        data = pcurs.fetchone()[0]

    with worldconn() as wconn:
        wconn.text_factory = str
        wcurs = wconn.cursor()
        wcurs.execute("UPDATE resource SET data = ? WHERE id = ?", (data, resource_id))
        wconn.commit()

def installPlugin(worldconn, pluginconn, server_version):
    plugin_infos = tcplug.getInfos(pluginconn())

    with worldconn() as conn:
        curs = conn.cursor()
        curs.execute(
            "SELECT id, build_hash FROM plugin WHERE identifier = ?",
            (plugin_infos['identifier'],)
        )
        res = curs.fetchone()
        if res is None:
            print "Install new plugin %(identifier)s" % plugin_infos
            curs.execute(
                "INSERT INTO plugin (id, build_hash, fullname, identifier) VALUES (NULL, ?, ?, ?)",
                (plugin_infos['build_hash'], plugin_infos['fullname'], plugin_infos['identifier'])
            )
            plugin_id = curs.lastrowid
            conn.commit()
        else:
            plugin_id = res[0]
            build_hash = res[1]
            if plugin_infos['build_hash'] != build_hash:
                print "Update plugin %(identifier)s" % plugin_infos
            else:
                print "plugin %(identifier)s is up-to-date" % plugin_infos
                return
        del curs

    with pluginconn() as pconn:
        pcurs = pconn.cursor()
        pcurs.execute("SELECT name, lua FROM map")
        maps = pcurs.fetchall()

    with worldconn() as wconn:
        wcurs = wconn.cursor()
        for name, lua in maps:
            wcurs.execute("INSERT INTO map (name, plugin_id, lua) VALUES (?, ?, ?)", (name, plugin_id, lua))

    with worldconn() as wconn:
        wcurs = wconn.cursor()
        wcurs.execute("SELECT filename, data_hash FROM resource")
        world_resources = dict((f, h) for f, h in wcurs)

    with pluginconn() as pconn:
        pcurs = pconn.cursor()
        pcurs.execute("SELECT filename, data_hash FROM resource")
        plugin_resources = pcurs.fetchall()

    for filename, data_hash in plugin_resources:
        if filename not in world_resources:
            insertResource(worldconn, pluginconn, plugin_id, filename, server_version)
        elif data_hash != plugin_infos['build_hash']:
            updateResource(worldconn, pluginconn, plugin_id, filename, server_version)
        else:
            print "File '%s' is up-to-date" % filename

    for obj in ['cube', 'entity']:
        with pluginconn() as pconn:
            pcurs = pconn.cursor()
            pcurs.execute("SELECT name, lua FROM %s_type" % obj)
            with worldconn() as wconn:
                wcurs = wconn.cursor()
                for name, lua in pcurs:
                    wcurs.execute(
                        "SELECT id FROM %s_type WHERE plugin_id = ? AND name = ?" % obj,
                        (plugin_id, name)
                    )
                    res = wcurs.fetchone()
                    if res is None:
                        print "Insert new %s type %s" % (obj, name)
                        wcurs.execute(
                            """
                                INSERT INTO %s_type (id, plugin_id, name, lua)
                                VALUES (NULL, ?, ?, ?)
                            """ % obj,
                            (plugin_id, name, lua)
                        )
                    else:
                        print "Update %s type %s" % (obj, name)
                        wcurs.execute(
                            """
                                UPDATE %s_type
                                    SET lua = ?
                                WHERE id = ?
                            """ % obj,
                            (lua, res[0])
                        )
                wconn.commit()

def finalizeWorld(worldconn, fullname, identifier, format_version):
    conn = worldconn()
    curs = conn.cursor()
    curs.execute("DELETE FROM world")
    curs.execute("SELECT max(version) FROM resource")
    version = curs.fetchone()[0]
    curs.execute(
        """
            INSERT INTO world (version, fullname, identifier, format_version)
            VALUES (?, ?, ?, ?)
        """,
        (version, fullname, identifier, format_version)
    )
    conn.commit()
    print "World '%s' successfully finalized" % fullname
