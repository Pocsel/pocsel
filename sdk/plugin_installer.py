#!/usr/bin/env python
# -*- encoding: utf-8 -*-

import argparse
import os
import sqlite3
import sys
import time
import traceback

from sdk import tools, tcworld
from sdk.constants import FORMAT_VERSION

world_tables = [
    (
        'plugin',
        [
            "id INTEGER PRIMARY KEY",
            "build_hash TEXT",
            "fullname TEXT",
            "identifier TEXT",
        ]
    ),
    (
        'resource',
        [
            "id INTEGER PRIMARY KEY",
            "plugin_id INTEGER",
            "version INTEGER",
            "filename TEXT",
            "type TEXT",
            "data_hash TEXT",
            "data BLOB",
        ]
    ),
    (
        'cube_type',
        [
            "id INTEGER PRIMARY KEY",
            "plugin_id INTEGER",
            "name TEXT",
            "lua TEXT",
        ]
    ),
    (
        'entity_file',
        [
            "plugin_id INTEGER",
            "name TEXT",
            "lua TEXT",
        ]
    ),
    (
        'world',
        [
            "version INTEGER",
            "fullname TEXT",
            "format_version INTEGER",
            "identifier TEXT",
            "build_hash TEXT",
        ]
    ),
    (
        'map',
        [
            "name TEXT",
            "plugin_id INTEGER",
            "lua TEXT",
            "tick INTEGER",
        ]
    ),
]

def main():
    parser = argparse.ArgumentParser(description='Install plugin files into a world')
    parser.add_argument('world_file', nargs='?', default="world.tcworld",
                        help="World file to update or create (defaults to world.tcworld)")
    parser.add_argument('plugins_dir', nargs='?', default='.',
                        help="Plugins directory to inspect (defaults to .)")
    parser.add_argument('--fullname', dest='fullname', default=None, help="Nice full name of the world")
    parser.add_argument('--identifier', dest='id', default=None,
                        help="Specify explicitly the tcworld identifier (defaults to world filename)")
    parser.add_argument('--recreate', action='store_true', help="Erase world data and recreate it from scratch")
    args = parser.parse_args()

    world_file = os.path.abspath(args.world_file)
    plugins_dir = os.path.abspath(args.plugins_dir)

    if not os.path.isdir(plugins_dir):
        raise Exception("'%s' is not a valid directory" % plugins_dir)
    if args.recreate and os.path.exists(world_file):
        os.unlink(world_file)

    if os.path.exists(world_file):
        conn = sqlite3.connect(world_file)
        curs = conn.cursor()
        curs.execute("SELECT version, fullname, identifier, format_version FROM world")
        res = curs.fetchone()
        if res is None:
            raise Exception("Unexpected empty world, erase the file, or repear the world table")
        version = res[0]
        fullname = res[1]
        identifier = res[2]
        format_version = res[3]
        if format_version != FORMAT_VERSION:
            raise Exception("World version is different from this sdk")
    else:
        tools.prepareDatabase(sqlite3.connect(world_file), world_tables)
        identifier = os.path.basename(world_file)
        identifier = '.'.join(filter(lambda s: s != 'tcworld', identifier.split('.')))
        fullname = identifier
        version = 0
        format_version = FORMAT_VERSION

    fullname = args.fullname or fullname
    identifier = args.id or identifier

    plugins = filter(lambda f: f.endswith('.tcplug'), os.listdir(plugins_dir))
    print "--- %s world '%s' ---" % (version == 0 and "Create" or "Update", fullname)
    print "Identifier:", identifier
    print "Full name:", fullname
    print "World file:", world_file
    print "Plugins to install:", ', '.join(plugins)
    print "--------"

    version += 1

    worldconn = lambda: sqlite3.connect(world_file)
    for plugin in plugins:
        pluginconn = lambda: sqlite3.connect(os.path.join(plugins_dir, plugin))
        tcworld.installPlugin(worldconn, pluginconn, version)

    build_hash = str(time.time())
    tcworld.finalizeWorld(worldconn, fullname, identifier, format_version, build_hash)

if __name__ == '__main__':
    try:
        main()
    except Exception, e:
        print "%s: An error occured: %s" % (sys.argv[0], str(e))
        traceback.print_exc()
        sys.exit(1)
