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
        latitude.append(dt[0])
        longitude.append(dt[1])
        temp.append(dt[2])
        humid.append(dt[3])
        light.append(dt[4])
        moist.append(dt[5])

    N = len(data)
    random_x = np.linspace(0, 1*N, N)

    columns = {}
    for k in range(N-1):
    	columns['x{}'.format(k + 1)] = random_x[:k + 1]
    	columns['y{}'.format(k + 1)] = light[:k + 1]

    data = [dict(
    	type = 'scatter',
        x = random_x,
        y = temp,
        name = 'Light',
        fill = 'tozeroy',
        fillcolor = '#B8F7D4',
        line = dict(
            color = ('#3A4750'),
            width = 4)
    )]

    axis=dict(ticklen=4,
              mirror=True,
              zeroline=False,
              showline=True,
              autorange=False,
              showgrid=False)

    layout = dict(title='Light Area Plot',
                  font=dict(family='Balto'),
                  showlegend=False,
                  autosize=False,
                  width=800,
                  height=400,
                  xaxis=dict(axis, **{'nticks':12, 'tickangle':-45,
                                      'range': [0,N]}),
                  yaxis=dict(axis, **{'title': 'Degrees Light', 'range':[0,1]}),
                  updatemenus=[dict(type='buttons',
                                    showactive=False,
                                    xanchor='left',
                                    yanchor='top',
                                    y = 1.3,
                                    x = 0,
                                    pad=dict(t=0, r=10),
                                    buttons=[dict(label='Play',
                                                  method='animate',
                                                  args=[None, dict(frame=dict(duration=20, redraw=False), 
                                                                   transition=dict(duration=30),
                                                                   fromcurrent=True,
                                                                   mode='immediate')])])])

    frames=[dict(data=[dict(x= columns['x{}'.format(k + 1)],
    						y= columns['y{}'.format(k + 1)],
    			)
    ]) for k in range(N-1)]

    fig=dict(data=data, layout=layout, frames=frames)
    plot_div = py.offline.plot(fig, output_type = 'div')
    return plot_div

