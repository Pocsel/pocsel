#!/usr/bin/env python2
# -*- encoding: utf-8 -*-

import argparse
import os
import sqlite3
import sys
import time
import traceback

from sdk import tools, tcplug

plugin_tables = [
    (
        'plugin',
        ["build_hash TEXT", "fullname TEXT", "identifier TEXT", "format_version INTEGER"]
    ),
    (
        'resource',
        ["filename TEXT", "type TEXT", "data_hash TEXT", "data BLOB"]
    ),
    (
        'cube_type',
        ["name TEXT", "lua TEXT"],
    ),
    (
        'entity_type',
        ["name TEXT", "lua TEXT"],
    ),
    (
        'map',
        ["name TEXT", "lua TEXT"],
    ),
]

def main():
    parser = argparse.ArgumentParser(description='Package development files into a distributable tcplug')
    parser.add_argument('source_dir', help="Your development files")
    parser.add_argument('dest_dir', default='.', nargs='?', help="Will create the file in that directory")
    parser.add_argument('--fullname', dest='fullname', default=None, help="Nice full name of the plugin")
    parser.add_argument('--identifier', dest='id', default=None, help="Specify explicitly the tcplug identifier")

    args = parser.parse_args()

    source_directory = os.path.abspath(args.source_dir)
    if not os.path.isdir(source_directory):
        raise Exception("source_dir is not a valid directory")

    dest_directory = os.path.abspath(args.dest_dir)
    if not os.path.isdir(dest_directory):
        raise Exception("dest_dir is not a valid directory")

    identifier = args.id or os.path.basename(source_directory)
    if len(identifier) < 2:
        raise Exception("Invalid identifier")

    identifier = '.'.join(filter(lambda s: s != 'tcplug', identifier.split('.')))
    fullname = args.fullname or identifier

    plugin_path = os.path.join(dest_directory, identifier + '.tcplug')
    print '--- Packaging plugin "%s" ---' % fullname
    print "Source dir:", source_directory
    print "Plugin path:", plugin_path
    print "Plugin identifier:", identifier
    print "Full plugin name:", fullname
    if os.path.exists(plugin_path):
        print "WARNING: The file '%s' will be overwrited" % plugin_path
    print '------'
    if os.path.exists(plugin_path):
        os.unlink(plugin_path)
    def connect(path):
        return sqlite3.connect(path)

    tools.prepareDatabase(connect(plugin_path), plugin_tables)
    tcplug.preparePlugin(lambda: connect(plugin_path), source_directory)

    build_hash = str(time.time())
    tcplug.finalizePlugin(connect(plugin_path), fullname, identifier, build_hash)
    print "--- plugin '%s' successfully packaged in '%s' ---" % (fullname, plugin_path)



if __name__ == '__main__':
    try:
        main()
    except Exception, e:
        print "%s: An error occured: %s" % (sys.argv[0], str(e))
        traceback.print_exc()
        sys.exit(1)
