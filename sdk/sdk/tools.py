# -*- encoding: utf-8 -*-

def hasTable(conn, table):
    curs = conn.cursor()
    curs.execute("""
        SELECT 1 FROM sqlite_master
        WHERE name = ?
    """, (table,))
    return curs.fetchone() is not None

def createTable(conn, table_name, columns):
    if not hasTable(conn, table_name):
        print "Create table", table_name
        request = "CREATE TABLE %s (%s)" % (table_name, ', '.join(columns))
        curs = conn.cursor()
        curs.execute(request)

def prepareDatabase(conn, tables):
    for table_name, columns in tables:
        createTable(conn, table_name, columns)
    conn.commit();


def getFileType(abspath):
    ext = abspath.split('.')[-1].lower()
    return {
        'lua': 'lua',
        'png': 'image',
        'md3': 'model',
        'fx': 'effect',
    }[ext]

