import plotly.plotly as py
import plotly.graph_objs as go

import requests
import numpy as np

URL = "http://608dev.net/sandbox/sc/garciag/all_dataHandler.py"
r = requests.get(url=URL)
data = eval(r.text)

humid1 = data[3]
humid2 = data[4]
humid3 = data[5]

N = len(data)
random_x = np.linspace(0, N*10, N)

# Create traces
trace0 = go.Scatter(
    x = random_x,
    y = humid1,
    name = 'Plot 1',
    mode = 'lines+markers'
)
trace1 = go.Scatter(
    x = random_x,
    y = humid2,
    name = 'Plot 2',
    mode = 'lines+markers'
)
trace2 = go.Scatter(
    x = random_x,
    y = humid3,
    name = 'Plot 3',
    mode = 'lines+markers'
)

data = [trace0, trace1, trace2]
layout = dict(title = 'Humidity',xaxis = dict(title = 'Time (min)'), yaxis = dict(title = 'Percentage'))
fig = go.Figure(data = data, layout = layout)
py.plot(fig)
