import plotly.graph_objs as go
from plotly import tools
import plotly as py
import requests
import numpy as np

def request_handler(r):
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
        if dt[2]<-100:
            continue
        else:
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
    trace1 = go.Scatter(
        x = random_x,
        y = humid,
        name = 'Humidity',
        line = dict(
            color = ('rgb(234, 82, 111)'),
            width = 4)
    )
    trace2 = go.Scatter(
        x = random_x,
        y = light,
        name = 'Light',
        line = dict(
            color = ('rgb(247,179,43)'),
            width = 4)
    )
    trace3 = go.Scatter(
        x = random_x,
        y = moist,
        name = 'Soil Moisture',
        line = dict(
            color = ('rgb(108, 193, 255)'),
            width = 4)
    )


    fig = tools.make_subplots(rows=2, cols=2, print_grid=False, subplot_titles=('Temperature vs Time', 'Humidity vs Time',
                                                          'Light vs Time', 'Soil Moisture vs Time'))

    fig.append_trace(trace0, 1, 1)
    fig.append_trace(trace1, 1, 2)
    fig.append_trace(trace2, 2, 1)
    fig.append_trace(trace3, 2, 2)

    fig['layout'].update(height=800, width=800, title='Subplots of Params')
    plot_div = py.offline.plot(fig, output_type = 'div')
    return plot_div

    # data2 = [trace0, trace1, trace2]

    # py.plot(data2, filename='line-mode')
