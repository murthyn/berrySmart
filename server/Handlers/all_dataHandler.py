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

	temp1 = []
	temp2 = []
	temp3 = []
	temp4 = []

	humid1 = []
	humid2 = []
	humid3 = []
	humid4 = []

	light1 = []
	light2 = []
	light3 = []
	light4 = []

	moist1 = []
	moist2 = []
	moist3 = []
	moist4 = []

	for  i in range(len(data)):
		try:
			esp1 = ast.literal_eval(data[i][0].split('#')[0])
			esp2 = ast.literal_eval(data[i][0].split('#')[1])
			esp3 = ast.literal_eval(data[i][0].split('#')[2])
			esp4 = ast.literal_eval(data[i][0].split('#')[3])

			if esp1['temp']<-100 or esp2['temp']<-100 or esp3['temp']<-100 or esp4['temp']<-100:
				pass 
			elif esp1['humid']<-100 or esp2['humid']<-100 or esp3['humid']<-100 or esp4['humid']<-100:
				pass
			else:
				temp1.append(esp1['temp'])
				temp2.append(esp2['temp'])
				temp3.append(esp3['temp'])
				temp4.append(esp4['temp'])


				humid1.append(esp1['humid'])
				humid2.append(esp2['humid'])
				humid3.append(esp3['humid'])
				humid4.append(esp4['humid'])

				light1.append(esp1['light'])
				light2.append(esp2['light'])
				light3.append(esp3['light'])
				light4.append(esp4['light'])

				moist1.append(esp1['soil_moisture'])
				moist2.append(esp2['soil_moisture'])
				moist3.append(esp3['soil_moisture'])
				moist4.append(esp4['soil_moisture'])
		except:
			pass

	return [temp1,temp2,temp3,temp4,humid1,humid2,humid3,humid4,light1,light2,light3,light4,moist1,moist2,moist3,moist4]
