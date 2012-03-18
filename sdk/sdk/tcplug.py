# -*- encoding: utf-8 -*-

import os
import hashlib
import sqlite3

from sdk import tools
from sdk.constants import FORMAT_VERSION

def prepareType(conn, type, name, lua):
    print "Add %s type '%s'" % (type, name)
    curs = conn.cursor()
    curs.execute("INSERT INTO %s (name, lua) VALUES (?, ?)" % type, (name, lua))
    conn.commit()

def addResource(connector, root, relpath, filename):
    abspath = os.path.join(root, filename)
    filename = os.path.join(relpath, filename).replace('\\', '/')
    try:
        file_type = tools.getFileType(abspath)
    except:
        print "Ignoring unknown resource", abspath
        return
    print "Add resource", filename
    conn = connector()
    curs = conn.cursor()
    curs.execute("INSERT INTO resource (filename, type) VALUES (?, ?)", (filename, file_type))
    conn.commit()

    conn = connector()
    conn.text_factory = str
    curs = conn.cursor()
    with open(abspath, 'rb') as f:
        data = f.read()
    data_hash = hashlib.md5(data).hexdigest()
    curs.execute("UPDATE resource SET data = ?, data_hash = ? WHERE filename = ?", (buffer(data), data_hash, filename))
    conn.commit()

def prepareMap(conn, name, lua):
    with conn:
        curs = conn.cursor()
        curs.execute("INSERT INTO map (name, lua) VALUES (?, ?)", (name, lua))

def preparePlugin(connector, plugin_dir):
    entities_dir = os.path.join(plugin_dir, 'entities')
    nb_entity = 0
    if os.path.isdir(entities_dir):
        entities = os.listdir(entities_dir)
        for entity in entities:
            if entity.endswith('.lua'):
                with open(os.path.join(entities_dir, entity)) as f:
                    data = f.read()
                prepareType(connector(), 'entity_file', entity[:-4], data)
                nb_entity += 1
            else:
                print "ignoring '%s' in entities folder" % entity
        if nb_entity == 0:
            print "No entity type in this plugin"
        else:
            print "Found %d entity types" % nb_entity
    else:
        print "No 'entities' directory"

    cubes_dir = os.path.join(plugin_dir, 'cubes')
    nb_cubes = 0
    if os.path.isdir(cubes_dir):
        cubes = os.listdir(cubes_dir)
        for cube in cubes:
            if cube.endswith('.lua'):
                with open(os.path.join(cubes_dir, cube)) as f:
                    data = f.read()
                prepareType(connector(), 'cube_type', cube[:-4], data)
                nb_cubes += 1
            else:
                print "ignoring '%s' in cubes folder" % cube
        if nb_cubes == 0:
            print "No cube type in this plugin"
        else:
            print "Found %d cube types" % nb_cubes
    else:
        print "No 'cubes' directory"


    resources_dir = os.path.join(plugin_dir, 'resources')
    nb_resources = 0
    if os.path.isdir(resources_dir):
        for root, dirs, files in os.walk(resources_dir, topdown=True):
            relpath = os.path.relpath(root, start=resources_dir)
            for filename in files:
                addResource(connector, root, relpath, filename)
                nb_resources += 1
        if nb_resources == 0:
            print "No resource in this plugin"
        else:
            print "Found %d resources" % nb_resources
    else:
        print "No 'resources' directory"

    maps_dir = os.path.join(plugin_dir, 'maps')
    nb_maps = 0
    if os.path.isdir(maps_dir):
        maps_files = filter(lambda f: f.endswith('.lua'), os.listdir(maps_dir))
        for m in maps_files:
            with open(os.path.join(maps_dir, m)) as f:
                data = f.read()
            prepareMap(connector(), m.split('.')[0], data)
            nb_maps += 1
        print "Found %d maps" % nb_maps
    else:
        print "No 'maps' directory"

def finalizePlugin(conn, fullname, identifier, build_hash):
    curs = conn.cursor()
    curs.execute(
        """
            INSERT INTO plugin (build_hash, fullname, identifier, format_version)
            VALUES (?, ?, ?, ?)
        """,
        (build_hash, fullname, identifier, FORMAT_VERSION)
    )
    conn.commit()

def getInfos(conn):
    curs = conn.cursor()
    curs.execute("SELECT build_hash, fullname, identifier, format_version FROM plugin")
    res = curs.fetchone()
    if res is None:
        raise Exception("Plugin file is not valid")
    return {
        'build_hash': res[0],
        'fullname': res[1],
        'identifier': res[2],
        'format_version': res[3],
    }
