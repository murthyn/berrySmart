import plotly.plotly as py
import plotly.graph_objs as go

import requests
import numpy as np

URL = "http://608dev.net/sandbox/sc/garciag/all_dataHandler.py"
r = requests.get(url=URL)
data = eval(r.text)

light1 = data[6]
light2 = data[7]
light3 = data[8]

N = len(data)
random_x = np.linspace(0, 10*N, N)

# Create traces
trace0 = go.Scatter(
    x = random_x,
    y = light1,
    name = 'Plot 1',
    mode = 'lines+markers'
)
trace1 = go.Scatter(
    x = random_x,
    y = light2,
    name = 'Plot 2',
    mode = 'lines+markers'
)
trace2 = go.Scatter(
    x = random_x,
    y = light3,
    name = 'Plot 3',
    mode = 'lines+markers'
)

data = [trace0, trace1, trace2]
layout = dict(title = 'Light',xaxis = dict(title = 'Time (min)'), yaxis = dict(title = 'Normalised Amperes'))
fig = go.Figure(data = data, layout = layout)
py.plot(fig)

    