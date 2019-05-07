import requests
import numpy as np
import random
import plotly.graph_objs as go
from plotly import tools
import plotly as py
import csv
import pandas as pd

def request_handler(r):

	mapbox_access_token = 'pk.eyJ1IjoiZ2FyY2lhZzIiLCJhIjoiY2p2MnUwaW92Mjk0aDQzc2syZWZ5bzIzZCJ9.MBF-GscHHLd5BZEsxnl_rA'

	#getting the data from database
	URL = "http://608dev.net/sandbox/sc/garciag/berryHandler_v2.py"
	r = requests.get(url=URL)
	data = eval(r.text)
	latitude = []
	longitude = []
	temp = []
	humid = []
	light = []
	moist = []

	#list of test coordinates
	coords = [(52.271949,4.520558),(52.270885,4.520226), (52.271200,4.517059), (52.272349,4.516951)]

	#generating lists organised by paramter in database
	i = 0
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
		i +=1
		j+=1

	#writing a csv file
	with open('berry_data.csv', 'w') as csvfile:
	    filewriter = csv.writer(csvfile, delimiter=',',quotechar='|')
	    filewriter.writerow(['Lat','Lon','Temp','Humid','Light','Moist'])
	    for i in range(4): #to get latest 4 values
	    	filewriter.writerow([latitude[i], longitude[i], temp[i], humid[i], light[i], moist[i]])

	df = pd.read_csv('berry_data.csv')

	#Graphing maps using mapbox
	scl = [ [0.0,"rgb(5, 10, 172)"],[0.35,"rgb(40, 60, 190)"],[0.5,"rgb(70, 100, 245)"],\
		[0.6,"rgb(90, 120, 245)"],[0.7,"rgb(106, 137, 247)"],[1.0,"rgb(220, 220, 220)"] ]

	data = [
		go.Scattermapbox(
			lat=df['Lat'],
			lon=df['Lon'],
			mode='markers+text',
			text = df['Moist'].astype(str).str[0:5],
			textposition='middle center',
	    	textfont=dict(
		        family='sans serif',
		        size=15,
		        color='#00cd00'
	    ),
			marker=dict(
				size=50,
				color = df['Moist'],
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
					ticksuffix = "unit??",
					dtick = 0.1,
					title="Soil Moisture Levels"
			),
			)
		)
	]

	layout = go.Layout(
		autosize=True,
		hovermode='closest',
		title = "Soil Moisture Mapped!",
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
	plot_div = py.offline.plot(fig, output_type = 'div')
	return plot_div
