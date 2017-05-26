#! /usr/bin/env python3
# -*- coding: utf-8 -*-

import sqlite3 as db
import sys
import utils
from debug import *

help_mes = "./updmark.py <Which_title> <which_mark>"

def add_mark(table_name, cl_args):
    conn = db.connect('bookmarks.s3db')
    cur = conn.cursor()
    
    utils.validate(cl_args)
    upd_id = utils.check_title(table_name, cl_args[0])
    query = """
    UPDATE {0} SET will = {1} WHERE ID = {2}
    """.format(table_name, cl_args[1], upd_id)
    start_log()
    
    log_print(query)
    cur.execute(query)
    #Verification of table name must be done outside
    conn.commit()
    conn.close()
    
if __name__ == "__main__":    
    cl_args = sys.argv[1:]
    length = len(cl_args)
    if not 2 == length:
        print("Wrong number of parameters")
        quit()
    
    #try
    add_mark('films', cl_args)
    #except
    #print possible reasons
    end_log()
