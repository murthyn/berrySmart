import requests
import numpy as np
import random
import plotly.plotly as py
import plotly.graph_objs as go
import csv
import pandas as pd

mapbox_access_token = 'pk.eyJ1IjoiZ2FyY2lhZzIiLCJhIjoiY2p2MnUwaW92Mjk0aDQzc2syZWZ5bzIzZCJ9.MBF-GscHHLd5BZEsxnl_rA'

#getting the data from database
URL = "http://608dev.net/sandbox/sc/garciag/all_dataHandler.py"
r = requests.get(url=URL)
data = eval(r.text)

light1 = data[6]
light2 = data[7]
light3 = data[8]

latitude = []
longitude = []

light = []
light.extend(light1)
light.extend(light2)
light.extend(light3)

#list of test coordinates
coords = [(52.271949,4.520558),(52.270885,4.520226), (52.272349,4.516951)]

#generating lists organised by paramter in database
i = 0
j = 0
for dt in data:
	if i>2:
		i = 0
	else:
		latitude.append(coords[i][0])
		longitude.append(coords[i][1])
		i +=1
		
#writing a csv file
with open('berry_data.csv', 'w') as csvfile:
    filewriter = csv.writer(csvfile, delimiter=',',quotechar='|')
    filewriter.writerow(['Lat','Lon','Light'])
    for i in range(3): #to get latest 4 values
    	filewriter.writerow([latitude[i], longitude[i], light[i]])

df = pd.read_csv('berry_data.csv')

#Graphing maps using mapbox
scl = [ [0.0,"#ffffff"],[0.35,"#b2b2b2"],[0.5,"#808080"],\
	[0.6,"#4c4c4c"],[0.7,"#333333"],[1.0,"#000000"] ]

data = [
	go.Scattermapbox(
		lat=df['Lat'],
		lon=df['Lon'],
		mode='markers+text',
		text = df['Light'].astype(str).str[0:5],
		textposition='middle center',
    	textfont=dict(
	        family='sans serif',
	        size=15,
	        color='#0000ff'
    ),
		marker=dict(
			size=50,
			color = df['Light'],
			opacity = 0.8,
			reversescale = True,
			colorscale = scl,
			colorbar = dict(
				thickness = 30,
				titleside = "right",
				outlinecolor = "rgba(68, 68, 68, 0)",
				ticks = "outside",
				ticklen = 3,
				showticksuffix = "last",
				ticksuffix = "Amperes",
				dtick = 0.1,
				title="Light Levels"
		),
		)
	)
]

layout = go.Layout(
	autosize=True,
	hovermode='closest',
	title = "Light Mapped",
	mapbox=go.layout.Mapbox(
		accesstoken=mapbox_access_token,
		bearing=0,
		center=go.layout.mapbox.Center(
			lat=52.271542,
			lon=4.518904
		),
		pitch=0,
		zoom=16,
		style = 'satellite'
	),
)

fig = go.Figure(data=data, layout=layout)
py.plot(fig)
