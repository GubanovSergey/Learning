#! /usr/bin/env python3
# -*- coding: utf-8 -*-

import sqlite3 as db
import sys
import utils
from debug import *

help_mes = "./show.py [top_n, all by default]"
#may add find by string

def show(table_name, number = None):
    conn = db.connect('bookmarks.s3db')
    cur = conn.cursor()
    
    query = "SELECT title, addit_info FROM {0} ORDER BY will DESC".format(table_name)
    if not number is None:
        utils.validate(number)
        query += " LIMIT {0}".format(number)
        
    start_log()
    log_print(query)
    
    cur.execute(query)
    print("Your top currently:\n")
    for row in cur:
        print(row[0], row[1])
    #Verification of table name must be done outside
    conn.commit()
    conn.close()
    
if __name__ == "__main__":    
    cl_args = sys.argv[1:]
    length = len(cl_args)
    if length > 1:
        print("Wrong number of parameters")
        quit()
        
    number = None
    if length:
        number = int(cl_args[0])
    #try
    show('films', number)
    #except
    #print possible reasons
    end_log()
