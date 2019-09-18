import plotly.graph_objects as go

import requests
import numpy as np

URL = "http://608dev.net/sandbox/sc/garciag/all_dataHandler.py"
r = requests.get(url=URL)
data = eval(r.text)

light1 = data[6]
light2 = data[7]
light3 = data[8]
light4 = data[9]

N = len(data)
random_x = np.linspace(0, 10*N, N)

# Create traces
trace0 = go.Scatter(
    x = random_x,
    y = light1,
    name = 'Node 1',
    mode = 'lines+markers'
)
trace1 = go.Scatter(
    x = random_x,
    y = light2,
    name = 'Node 2',
    mode = 'lines+markers'
)
trace2 = go.Scatter(
    x = random_x,
    y = light3,
    name = 'Node 3',
    mode = 'lines+markers'
)

trace3 = go.Scatter(
    x = random_x,
    y = light4,
    name = 'Node 4',
    mode = 'lines+markers'
)

data = [trace0, trace1, trace2,trace3]
layout = dict(title = 'Light',xaxis = dict(title = 'Time (min)'), yaxis = dict(title = 'Normalised Amperes'))
fig = go.Figure(data = data, layout = layout)
fig.show()

    