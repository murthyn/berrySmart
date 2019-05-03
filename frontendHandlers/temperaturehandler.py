import requests 
import json

def request_handler(req):
    # api-endpoint
    URL = "http://api.openweathermap.org/data/2.5/weather"

  
    # defining a params dict for the parameters to be sent to the API 
    PARAMS = {'id':4931972, 'appid':'2b9723d4047f9210c690f406d2969ccf'}
  
    # sending get request and saving the response as response object 
    r = requests.get(url = URL, params=PARAMS) 
  
    # extracting data in json format 
    data = r.json()
    return round(data['main']['temp']-273.15,2)
