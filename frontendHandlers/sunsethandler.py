import requests 
import json
from datetime import datetime

def request_handler(req):
    # api-endpoint
    URL = "http://api.openweathermap.org/data/2.5/weather"

  
    # defining a params dict for the parameters to be sent to the API 
    PARAMS = {'id':4931972, 'appid':'2b9723d4047f9210c690f406d2969ccf'}
  
    # sending get request and saving the response as response object 
    r = requests.get(url = URL, params=PARAMS) 
  
    # extracting data in json format 
    data = r.json()


    ts = int(data['sys']['sunset'])-5*60*60

    return datetime.utcfromtimestamp(ts).strftime(' %H:%M:%S')
