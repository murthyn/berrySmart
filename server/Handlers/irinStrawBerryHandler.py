import requests
import numpy as np
import statistics
from datetime import datetime
import ast

def request_handler(request):
  URL = "http://608dev.net/sandbox/sc/garciag/strawberryHandler.py"
  r = requests.get(url=URL)
  text = r.text
  data= eval(text)

  k = int(request['values']['k'])
  a = int(request['values']['a'])
          
  temp1 = []
  temp2 = []
  temp3 = []

  humid1 = []
  humid2 = []
  humid3 = []

  light1 = []
  light2 = []
  light3 = []

  moist1 = []
  moist2 = []
  moist3 = []
  
  data1 = []
  data2 = []
  data3 = []
  
  time1 = []
  time2 = []
  time3 = []

  for  i in range(len(data)):
    try:
      tempdate = data[i][1].split('.')[0]
      datetime_object = datetime.strptime(tempdate, '%Y-%m-%d %H:%M:%S')
      
      esp1 = ast.literal_eval(data[i][0].split('#')[0])
      esp2 = ast.literal_eval(data[i][0].split('#')[1])
      esp3 = ast.literal_eval(data[i][0].split('#')[2])

      temp1.append(esp1['temp'])
      temp2.append(esp2['temp'])
      temp3.append(esp3['temp'])

      humid1.append(esp1['humid'])
      humid2.append(esp2['humid'])
      humid3.append(esp3['humid'])

      light1.append(esp1['light'])
      light2.append(esp2['light'])
      light3.append(esp3['light'])

      moist1.append(esp1['soil_moisture'])
      moist2.append(esp2['soil_moisture'])
      moist3.append(esp3['soil_moisture'])
      
      time1.append(datetime_object)
      time2.append(datetime_object)
      time3.append(datetime_object)
    except:
      pass
  aggregate = []
  if a==1:
    for i in range(len(temp1)):
      aggregate.append(({'temp':temp1[i], 'humid':humid1[i], 'light':light1[i], 'soil_moisture':moist1[i]} ,time1[i]))
  elif a==2:
    for i in range(len(temp2)):
      aggregate.append(({'temp':temp2[i], 'humid':humid2[i], 'light':light2[i], 'soil_moisture':moist2[i]} ,time2[i]))
  elif a==3:
    for i in range(len(temp3)):
      aggregate.append(({'temp':temp3[i], 'humid':humid3[i], 'light':light3[i], 'soil_moisture':moist3[i]} ,time3[i]))
                       
                     
  if len(aggregate) >= 20:
    aggregate.sort(key = lambda x: x[1], reverse=True)
    aggregate = aggregate[: 20]
    
     
  if k==1:
    return round((np.mean(np.asarray([aggregate[i][0]['temp']*1.8+32 for i in range(len(aggregate))]))),2)
  elif k==5:
    return round((np.std(np.asarray([aggregate[i][0]['temp']*1.8+32 for i in range(len(aggregate))]))),2)
  elif k==2:
    return round(np.mean(np.asarray([aggregate[i][0]['humid'] for i in range(len(aggregate))])),2)
  elif k==6:
    return round(np.std(np.asarray([aggregate[i][0]['humid'] for i in range(len(aggregate))])),2)
  elif k==3:
    return round(np.mean(np.asarray([aggregate[i][0]['light'] for i in range(len(aggregate))])),2)
  elif k==7:
    return round(np.std(np.asarray([aggregate[i][0]['light'] for i in range(len(aggregate))])),2)
  elif k==4:
    return round(np.mean(np.asarray([aggregate[i][0]['soil_moisture'] for i in range(len(aggregate))])),2)
  elif k==8:
    return round(np.std(np.asarray([aggregate[i][0]['soil_moisture'] for i in range(len(aggregate))])),2)

