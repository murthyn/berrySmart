import plotly.graph_objs as go
from plotly import tools
import plotly as py
import requests
import numpy as np

URL = "http://608dev.net/sandbox/sc/garciag/all_dataHandler.py"
r = requests.get(url=URL)
data = eval(r.text)

humid1 = data[3]
humid2 = data[4]
humid3 = data[5]
humid = humid1+humid2+humid3


N = len(data)
random_x = np.linspace(0, N*10, N)

columns = {}
for k in range(N-1):
  columns['x{}'.format(k + 1)] = random_x[:k + 1]
  columns['y{}'.format(k + 1)] = humid[:k + 1]

data = [dict(
  type = 'scatter',
    x = random_x,
    y = humid,
    name = 'Humidity',
    fill = 'tozeroy',
    fillcolor = 'rgb(0, 200, 200)',
    line = dict(
        color = ('rgb(0, 0, 100)'),
        width = 4)
)]

axis=dict(ticklen=4,
          mirror=True,
          zeroline=False,
          showline=True,
          autorange=False,
          showgrid=False)

layout = dict(title='Humidity Area Plot',
              font=dict(family='Balto'),
              showlegend=False,
              autosize=False,
              width=800,
              height=400,
              xaxis=dict(axis, **{'nticks':12, 'tickangle':-45,
                                  'range': [0,N]}),
              yaxis=dict(axis, **{'title': 'Relative Percentage', 'range':[0,40]}),
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
py.offline.plot(fig)


