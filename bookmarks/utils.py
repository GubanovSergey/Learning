import sqlite3 as db
from debug import *

def validate(iterable):
    for str in iterable:
        assert(str.find(';') == -1)
        
def values_sub(n_args):
    return '?' + ',?' * (n_args-1)
        
def check_title(table_name, title): #returns ID of chosen film
    conn = db.connect('bookmarks.s3db')
    cur = conn.cursor()
    
    cur.execute("""
    SELECT id, title, addit_info FROM {0} WHERE title = "{1}"
    """.format(table_name, title))
    
    rows = cur.fetchall()
    if len(rows) > 1:
        print ("Ambiguous title. Please, choose one of the films and type its ID below\n")
        ids = []
        for row in rows:
            ids.append(row[0])
            print("ID=",row[0],"\tTitle=",row[1],"\tAdd_info=",row[2])
            
        chosen = int(input())
        while not chosen in ids:
            print("Incorrect ID. Try again\n")
            chosen = int(input())
    elif rows:
        chosen = rows[0][0]
    else:
        start_log()
        log_print("#No matching record found!")
        end_log()
    return chosen
    
