import sqlite3
import random
import string
import datetime


berry_db = '__HOME__/berry.db'


def request_handler(request):
     # SQL
    N=5
    conn = sqlite3.connect(berry_db)  # connect to that database (will create if it doesn't already exist)
    c = conn.cursor()  # make cursor into database (allows us to execute commands)
    c.execute('''CREATE TABLE IF NOT EXISTS berry_table (lat float, lon float, temp float, humid float, light float, timing timestamp);''') # run a CREATE TABLE command

    if request['method'] == 'GET':
        allthings = c.execute('''SELECT  lat,lon,temp,humid,light,timing FROM berry_table ORDER BY timing DESC;''').fetchall()
        things = None
        if len(allthings) >= N:
            things = allthings[: N]
            values = []
            for i in range(N):
                val = things[i]
                values.extend([val[0], val[1], val[2], val[3],val[4]])
            values = tuple(values)
                           
            return  ''' 
              <table style="width:100%" border="1">
                 <tr>
                   <th>Latitude</th>
                   <th>Longitude</th>
                   <th>Temperature</th> 
                   <th>Humidity</th>
                   <th>Light</th>
                </tr>
                <tr>
                   <td>{}</td>
                   <td>{}</td> 
                   <td>{}</td>
                   <td>{}</td>
                   <td>{}</td>
               </tr>
               <tr>
                   <td>{}</td>
                   <td>{}</td> 
                   <td>{}</td>
                   <td>{}</td>
                   <td>{}</td>
               </tr>
               <tr>
                   <td>{}</td>
                   <td>{}</td> 
                   <td>{}</td>
                   <td>{}</td>
                   <td>{}</td>
               </tr>
               <tr>
                   <td>{}</td>
                   <td>{}</td> 
                   <td>{}</td>
                   <td>{}</td>
                   <td>{}</td>
               </tr>
               <tr>
                   <td>{}</td>
                   <td>{}</td> 
                   <td>{}</td>
                   <td>{}</td>
                   <td>{}</td>
               </tr>
               </table>'''.format(things[0][0],things[0][1],things[0][2],things[0][3],things[0][4], \
                                  things[1][0],things[1][1],things[1][2],things[1][3],things[1][4], \
                                  things[2][0],things[2][1],things[2][2],things[2][3],things[2][4], \
                                  things[3][0],things[3][1],things[3][2],things[3][3],things[3][4], \
                                  things[4][0],things[4][1],things[4][2],things[4][3],things[4][4])
              
        else:
            return ''' Not enough values are there in the database'''
            

    # POST:
    else:
        lat = request['values']['lat']
        lon =request['values']['lon']
        temp = request['values']['temp']
        humid = request['values']['humid']
        light = request['values']['light']