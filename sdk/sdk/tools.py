# -*- encoding: utf-8 -*-

def hasTable(conn, table):
    curs = conn.cursor()
    curs.execute("""
        SELECT 1 FROM sqlite_master
        WHERE name = ?
    """, (table,))
    return curs.fetchone() is not None

def prepareDatabase(conn, tables):
    for table_name, columns in tables:
        if not hasTable(conn, table_name):
            print "Create table", table_name
            request = "CREATE TABLE %s (%s)" % (table_name, ', '.join(columns))
            curs = conn.cursor()
            curs.execute(request)
    conn.commit();


def getFileType(abspath):
    ext = abspath.split('.')[-1].lower()
    return {
        'lua': 'lua',
        'png': 'image',
        'md3': 'model',
    }[ext]

