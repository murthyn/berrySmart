import requests
import numpy as np
import random
import plotly.graph_objs as go
from plotly import tools
import plotly.plotly as py


URL = "http://608dev.net/sandbox/sc/garciag/berryHandler_v2.py"
r = requests.get(url=URL)
data = eval(r.text)
latitude = []
longitude = []
temp = []
humid = []
light = []
moist = []

coords = [(52.272212, 4.517091),(52.27071,4.519479), (52.271317,4.519979), (52.271341,4.516957)]
rand = random.uniform(-1,1)
i = 0
data_dict = {}

j = 0

for dt in data:
	if i>3:
		i = 0
	latitude.append(coords[i][0])
	longitude.append(coords[i][1])
	temp.append(dt[2])
	humid.append(dt[3])
	light.append(dt[4])
	moist.append(dt[5]/1000)

	data_dict[j] = {'Lat': latitude[j], 'Lon': longitude[j], 'temp':temp[j], 'humid': humid[j], 'light': light[j], 'moist': moist[j]}
	i +=1
	j+=1

N = len(data)
random_x = np.linspace(0, 1, N)

scl = [ [0,"rgb(5, 10, 172)"],[0.35,"rgb(40, 60, 190)"],[0.5,"rgb(70, 100, 245)"],\
	[0.6,"rgb(90, 120, 245)"],[0.7,"rgb(106, 137, 247)"],[1,"rgb(220, 220, 220)"] ]

data = [go.Scattergeo(
	lat = data_dict['Lat'],
	lon = data_dict['Lon'],
	marker = dict(
		color = data_dict['moist'],
		colorscale = scl,
		reversescale = True,
		autocolorscale = False,
		symbol = 'circle',
		opacity = 0.8,
		size = 20,		
		colorbar = dict(
			thickness = 30,
			titleside = "right",
			outlinecolor = "rgba(68, 68, 68, 0)",
			ticks = "outside",
			ticklen = 3,
			showticksuffix = "last",
			ticksuffix = "inches?",
			dtick = 0.1,
			title="Soil Moisture Levels"
		)				
	)
)]

layout = dict(
	geo = dict(
		scope = 'europe',
		showland = True,
		landcolor = "rgb(212, 212, 212)",
		subunitcolor = "rgb(255, 255, 255)",
		countrycolor = "rgb(255, 255, 255)",
		showlakes = True,
		lakecolor = "rgb(255, 255, 255)",
		showsubunits = True,
		showcountries = True,
		resolution = 50,
		projection = dict(
			type = 'orthographic')
	),
	title = 'US Precipitation 06-30-2015<br>Source: <a href="http://water.weather.gov/precip/">NOAA</a>',
)

fig = go.Figure(data=data, layout=layout )
py.plot(fig, filename='Soil Moisture Levels' )