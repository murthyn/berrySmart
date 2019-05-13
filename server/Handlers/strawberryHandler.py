import sqlite3
import random
import string
import datetime

    
strawberrydb = '__HOME__/strawberry.db'


def request_handler(request):
     # SQL
    conn = sqlite3.connect(strawberrydb)  # connect to that database (will create if it doesn't already exist)
    c = conn.cursor()  # make cursor into database (allows us to execute commands)
    c.execute('''CREATE TABLE IF NOT EXISTS strawberrytable (word str, timing timestamp);''') # run a CREATE TABLE command

    if request['method'] == 'GET':
        allthings = c.execute('''SELECT  word,timing FROM strawberrytable ORDER BY timing DESC;''').fetchall()
        things = []

        if len(allthings) >= 1:
            return allthings

        else:
            return ''' Not enough values are in the database'''
            

    # POST:
    else:
        text = request['form']['text']
                       
        c.execute('''INSERT into strawberrytable VALUES (?,?);''', (text,datetime.datetime.now()))
        
    conn.commit() # commit commands
    conn.close() # close connection to database