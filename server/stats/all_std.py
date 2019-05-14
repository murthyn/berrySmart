import requests
import numpy as np
import statistics
from datetime import datetime
import statistics

def request_handler(request):
	URL = "http://608dev.net/sandbox/sc/garciag/all_dataHandler.py"
	r = requests.get(url=URL)
	data = eval(r.text)

	temp1 = data[0]
	temp2 = data[1]
	temp3 = data[2]

	temp = []
	temp.extend(temp1)
	temp.extend(temp2)
	temp.extend(temp3)


	humid1 = data[3]
	humid2 = data[4]
	humid3 = data[5]

	humid = []
	humid.extend(humid1)
	humid.extend(humid2)
	humid.extend(humid3)

	light1 = data[6]
	light2 = data[7]
	light3 = data[8]

	light = []
	light.extend(light1)
	light.extend(light2)
	light.extend(light3)

	moist1 = data[9]
	moist2 = data[10]
	moist3 = data[11]

	moist = []
	moist.extend(moist1)
	moist.extend(moist2)
	moist.extend(moist3)

	k = int(request['values']['k'])

	if k ==1:
		return round(statistics.std(temp)*1.8+32,2)
	elif k == 2:
		return round(statistics.std(humid),2)
	elif k == 3:
		return round(statistics.std(light)*100,2)
	elif k ==4:
		return round(statistics.std(moist)*100,2)