import plotly.graph_objects as go

import requests
import numpy as np

URL = "http://608dev.net/sandbox/sc/garciag/all_dataHandler.py"
r = requests.get(url=URL)
data = eval(r.text)

humid1 = data[3]
humid2 = data[4]
humid3 = data[5]
humid4 = data[6]

N = len(data)
random_x = np.linspace(0, N*10, N)

# Create traces
trace0 = go.Scatter(
    x = random_x,
    y = humid1,
    name = 'Node 1',
    mode = 'lines+markers'
)
trace1 = go.Scatter(
    x = random_x,
    y = humid2,
    name = 'Node 2',
    mode = 'lines+markers'
)
trace2 = go.Scatter(
    x = random_x,
    y = humid3,
    name = 'Node 3',
    mode = 'lines+markers'
)

trace3 = go.Scatter(
    x = random_x,
    y = humid4,
    name = 'Node 4',
    mode = 'lines+markers'
)

data = [trace0, trace1, trace2,trace3]
layout = dict(title = 'Humidity',xaxis = dict(title = 'Time (min)'), yaxis = dict(title = 'Percentage'))
fig = go.Figure(data = data, layout = layout)
fig.show()
