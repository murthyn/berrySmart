import sqlite3
import random
import string
import datetime


berry3_db = '__HOME__/berry3.db'


def request_handler(request):
     # SQL
    conn = sqlite3.connect(berry3_db)  # connect to that database (will create if it doesn't already exist)
    c = conn.cursor()  # make cursor into database (allows us to execute commands)
    c.execute('''CREATE TABLE IF NOT EXISTS berry3_table (lat float, lon float, temp float, humid float, light float, moist float, timing timestamp);''') # run a CREATE TABLE command

    if request['method'] == 'GET':
        allthings = c.execute('''SELECT  lat,lon,temp,humid,light,moist,timing FROM berry3_table ORDER BY timing DESC;''').fetchall()
        things = None

        if len(allthings) >= 1:
          return allthings

        else:
            return ''' Not enough values are there in the database'''
            

    # POST:
    else:
        lat = request['form']['lat']
        lon =request['form']['lon']
        temp = request['form']['temp']
        humid = request['form']['humid']
        light = request['form']['light']
        moist = request['form']['moist']
               
        c.execute('''INSERT into berry3_table VALUES (?,?,?,?,?,?,?);''', (lat,lon,temp,humid,light,moist,datetime.datetime.now()))
        
    conn.commit() # commit commands
    conn.close() # close connection to database