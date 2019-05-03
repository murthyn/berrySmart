import requests
import sqlite3
login_db = '__HOME__/login.db'


def request_handler(request):
    if request['method']=='GET':
        conn = sqlite3.connect(login_db)  #connect to that database (will create if it doesn't already exist)
        c = conn.cursor()  #make cursor into database (allows us to execute commands)
        c.execute('''CREATE TABLE IF NOT EXISTS login_tbl (user text primary key,password text);''') # run a CREATE TABLE command
        ouruser=request['values']['user']
        row=c.execute('''SELECT * FROM login_tbl WHERE user=? ;''', (ouruser,)).fetchall()# run a CREATE TABLE command
        if len(row)==0:
            return 'fail'
        passworddb = row[0][1]
        conn.commit() #commit commands
        conn.close() #close connection to database
        return 'success' if passworddb == request['values']['password'] else 'fail'
    elif request['method']=='POST':
        conn = sqlite3.connect(login_db)  #connect to that database (will create if it doesn't already exist)
        c = conn.cursor()  #make cursor into database (allows us to execute commands)
        c.execute('''CREATE TABLE IF NOT EXISTS login_tbl (user text primary key, password text);''') # run a CREATE TABLE command
        ouruser=request['values']['user']
        ourpassword = request['values']['password']
        row=c.execute('''SELECT * FROM login_tbl WHERE user=? ;''', (ouruser,)).fetchall()# run a CREATE TABLE command
        if len(row)!=0:
            return 'fail'
        c.execute('''INSERT or IGNORE into login_tbl VALUES (?,?);''', (ouruser,ourpassword))
        conn.commit() #commit commands
        conn.close() #close connection to database
        return 'success'
