import requests
import numpy as np
import statistics


#get request for the server data
URL = "http://608dev.net/sandbox/sc/garciag/strawberryHandler.py"
r = requests.get(url=URL)
data = eval(r.text)
print(data)

latitude = []
longitude = []

temp = []
humid = []
light = []
moist = []

avg_temp = 0
avg_humid = 0
avg_light = 0
avg_moist = 0

std_temp = 0
std_humid = 0
std_light = 0
std_moist = 0

median_temp = 0
median_humid = 0
median_light = 0
media_moist = 0

for dt in data[60:]:
	if dt[2]<-100:
		continue
	else:
	    latitude.append(dt[0])
	    longitude.append(dt[1])
	    temp.append(dt[2])
	    humid.append(dt[3])
	    light.append(dt[4])
	    moist.append(dt[5])

avg_temp = statistics.mean(temp)
avg_humid = statistics.mean(humid)
avg_light = statistics.mean(light)
avg_moist = statistics.mean(moist)

std_temp = statistics.stdev(temp)
std_humid = statistics.stdev(humid)
std_light = statistics.stdev(light)
std_moist = statistics.stdev(moist)

median_temp = statistics.median(temp)
median_humid = statistics.median(humid)
median_light = statistics.median(light)
median_moist = -1*statistics.median(moist)

send = [[avg_temp,std_temp,median_temp], [avg_humid,std_humid,median_humid], [avg_light,std_light,median_light], [avg_moist,std_moist,median_moist]]
print(send)
	
