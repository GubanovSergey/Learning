#! /usr/bin/env python3
# -*- coding: utf-8 -*-

import sqlite3 as db
import sys
import utils
from debug import *

help_mes = "./adder.py <Which_title> [expectation mark = 5]"

class Adder:
    def __init__(self, table_name): #./adder.py 'Title' [expectation_mark]
    #[link]
        cl_args = sys.argv[1:]
        length = len(cl_args)
        if not 1 <= length <= 2:
            print("Wrong number of parameters")
            quit()
        
        self._conn = db.connect('bookmarks.s3db')
        cur = self._conn.cursor()

        title = cl_args[0]
        will = None
        if length == 2:
	        will = int(cl_args[1])
        
        subs, args = ("title", (title,)) if will is None else ("title, will", (title,will))

        query = "INSERT INTO {0}({1}) VALUES({2})".format(table_name, subs, utils.values_sub(len(args))) 
        
        start_log()
        log_print(query)
        log_print("#Args are {0}".format(args))
        try:           
            cur.execute(query, args)    
        except db.Error as e:
            print("""\nError! This title may be already present. If so, please add information by \n\
\t./addinfo "this_title" "Further info"\nfirst to distinguish these two records""")
            dbg_print("Error: {0}".format(e.args[0]))
            quit()
            
        self._conn.commit()

    def finalize(self):
        self._conn.close()
        
if __name__ == "__main__":
    addr = Adder('films')
    addr.finalize()     
    end_log() 
    

