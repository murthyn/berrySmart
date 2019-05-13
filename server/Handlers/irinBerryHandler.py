import requests
import numpy as np
import statistics
from datetime import datetime

def request_handler(request):
  URL = "http://608dev.net/sandbox/sc/garciag/strawberryHandler.py"
  r = requests.get(url=URL)
  text = r.text
  data = eval(text)
  k = int(request['values']['k'])
  aggregate = []
  for  i in range(len(data)):
    try:
      temp = eval(data[i][0]) #temp is a dictionary
      tempdate = data[i][1].split('.')[0]
      datetime_object = datetime.strptime(tempdate, '%Y-%m-%d %H:%M:%S')
      aggregate.append((temp, datetime_object))
    except:
      pass
  a, b= None, None  
  if k==1:
    return round((np.mean(np.asarray([aggregate[i][0]['temp'] for i in range(len(aggregate))])))*1.8+32,2)
  elif k==5:
    return round((np.std(np.asarray([aggregate[i][0]['temp'] for i in range(len(aggregate))])))*1.8+32,2)
  elif k==2:
    return round(np.sum(np.asarray([aggregate[i][0]['humid'] for i in range(len(aggregate))]))/len(data),2)
  elif k==6:
    return round(np.std(np.asarray([aggregate[i][0]['humid'] for i in range(len(aggregate))]))/len(data),2)
  elif k==3:
    return round(np.sum(np.asarray([aggregate[i][0]['light'] for i in range(len(aggregate))]))/len(data),2)
  elif k==7:
    return round(np.std(np.asarray([aggregate[i][0]['light'] for i in range(len(aggregate))]))/len(data),2)
  elif k==4:
    return round(np.sum(np.asarray([aggregate[i][0]['soil_moisture'] for i in range(len(aggregate))]))/len(data),2)
  elif k==8:
    return round(np.std(np.asarray([aggregate[i][0]['soil_moisture'] for i in range(len(aggregate))]))/len(data),2)

#k1 = temp mean
#k2 = humid mean
#k3 = light mean
#k4 = soil moisture mean
#k5 = temp std
#k7 = humid std
#k8 = light std
#k9 = soil moisture std