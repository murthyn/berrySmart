import plotly.graph_objs as go
from plotly import tools
from plotly.grid_objs import Grid, Column
from plotly.tools import FigureFactory as FF 
import plotly as py
import requests
import numpy as np

URL = "http://608dev.net/sandbox/sc/garciag/berryHandler_v2.py"
r = requests.get(url=URL)
data = eval(r.text)
latitude = []
longitude = []
temp = []
humid = []
light = []
moist = []

for dt in data:
    latitude.append(dt[0])
    longitude.append(dt[1])
    temp.append(dt[2])
    humid.append(dt[3])
    light.append(dt[4])
    moist.append(dt[5])

N = len(data)
random_x = np.linspace(0, 1, N)

# Create traces
trace0 = go.Scatter(
    x = random_x,
    y = temp,
    name = 'Temperature',
    line = dict(
        color = ('rgb(155, 80, 148)'),
        width = 4)
)

data = [trace0]
layout = dict(title = 'Styled Scatter',
            yaxis = dict(zeroline = False),
            xaxis = dict(zeroline = False),
            sliders = dict(
                active = 0,
                yanchor = 'top',
                xanchor = 'left',
                transition = dict(
                    duration = 300, 
                    easing = 'cubic-in-out'),
                pad = dict(
                    b = 100,
                    t = 50),
                len = 0.9,
                x = 0.1,
                y = 0,
                steps = dict(
                    method = 'animate',
                    label = 'label-for-frame',
                    value = 'label-for-frame(defaults to label)',
                    steps = [])),
            updatemenus = [dict(
                buttons = [dict(
                    args = [None, dict(
                        frame = dict(
                            duration = 500,
                            redraw = False),
                        fromcurrent = True,
                        transition = dict(
                            duration = 300,
                            easing = 'quadratic-in-out'))],
                    label = 'Play',
                    method = 'animate')],
                direction = 'left',
                pad = dict(
                    r = 10,
                    t = 87),
                showactive = False,
                type = 'buttons',
                x = 0.1,
                xanchor = 'right',
                y = 0,
                yanchor = 'top')],
             )
            

fig = dict(data=data, layout=layout)
py.offline.plot(fig, filename='Slider Plotting.html')
# data2 = [trace0, trace1, trace2]

# py.plot(data2, filename='line-mode')