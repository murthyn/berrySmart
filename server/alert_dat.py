import requests
import numpy as np
import statistics

temp_limit = 23
humid_limit = 70
light_limit = 0.2
moist_limit = 0.3

#get request for the server data
URL = "http://608dev.net/sandbox/sc/garciag/mean_std.py"
r = requests.get(url=URL)
t = r.text.strip().split("\n")[0]
data = eval(t)


avg_temp = data[0][0]
avg_humid = data[1][0]
avg_light = data[2][0]
avg_moist = data[3][0]

std_temp = data[0][1]
std_humid = data[1][1]
std_light = data[2][1]
std_moist = data[3][1]

median_temp = data[0][2]
median_humid = data[1][2]
median_light = data[2][2]
media_moist = data[3][2]

alerts = []
if avg_temp>temp_limit:
	alerts.append("Too hot! Average temperature is greater than "+str(temp_limit))
else:
	alerts.append(None)

if avg_humid>humid_limit:
	alerts.append("Too humid! Average humidity is greater than "+str(humid_limit))
else:
	alerts.append(None)

if avg_light<light_limit:
	alerts.append("Too dark! Average light is greater than "+str(light_limit))
else:
	alerts.append(None)

if avg_moist<moist_limit:
	alerts.append("Too dry! Average soil moisture is greater than "+str(moist_limit))
else:
	alerts.append(None)

print(alerts)

