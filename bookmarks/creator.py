#! /usr/bin/env python3
# -*- coding: utf-8 -*-

import sqlite3 as db
import sys

help_mes = "./creator.py table_name\n"

class Creator:
    def __init__(self):
        possible_names = ['books', 'films', 'music']
        
        cl_args = sys.argv[1:]
        length = len(cl_args)
        if not 1 == length:
            print("Wrong number of parameters")
            quit()
        
        name = cl_args[0]
        if not name in possible_names:
            print("Unfortunately, you must choose from\n", possible_names)
            quit()
             
        self._conn = db.connect('bookmarks.s3db')
        cur = self._conn.cursor()
        cur.execute("""
        CREATE TABLE IF NOT EXISTS """ + name + """ (
            ID          INTEGER PRIMARY KEY AUTOINCREMENT,
            title       TEXT NOT NULL,
            addit_info  TEXT DEFAULT "",
            link        TEXT,
            will        INTEGER DEFAULT 5 CHECK(will >= 0 AND will < 11)
        )""")
#        cur.execute("""
 #       CREATE UNIQUE INDEX IF NOT EXISTS ID ON """ + name + """(title, addit_info)
  #      """)
        self._conn.commit()
        
    def finalize (self):
        self._conn.close()
        
if __name__ == "__main__":
    crt = Creator()
    crt.finalize()
