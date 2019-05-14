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

	humid1 = data[3]
	humid2 = data[4]
	humid3 = data[5]

	light1 = data[6]
	light2 = data[7]
	light3 = data[8]

	moist1 = data[9]
	moist2 = data[10]
	moist3 = data[11]

	k = int(request['values']['k'])

	if k ==1:
		return round(statistics.mean(temp1)*1.8+32)
	elif k == 2:
		return round(statistics.mean(temp2)*1.8+32)
	elif k ==3:
		return round(statistics.mean(temp3)*1.8+32)
	elif k == 4:
		return round(statistics.mean(humid1))
	elif k == 5:
		return round(statistics.mean(humid2))
	elif k == 6:
		return round(statistics.mean(humid3))
	elif k == 7:
		return round(statistics.mean(light1))
	elif k ==8:
		return round(statistics.mean(light2))
	elif k ==9:
		return round(statistics.mean(light3))
	elif k ==10:
		return round(statistics.mean(moist1))
	elif k ==11:
		return round(statistics.mean(moist2))
	elif k ==12:
		return round(statistics.mean(moist3))


