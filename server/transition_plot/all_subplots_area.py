import plotly.graph_objs as go
from plotly import tools
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
random_x = np.linspace(0, 1*N, N)

columns1 = {}
columns2 = {}
columns3 = {}
columns4 = {}

for k in range(N-1):
  columns1['x{}'.format(k + 1)] = random_x[:k + 1]
  columns1['y{}'.format(k + 1)] = humid[:k + 1]

  columns2['x{}'.format(k + 1)] = random_x[:k + 1]
  columns2['y{}'.format(k + 1)] = light[:k + 1] 

  columns3['x{}'.format(k + 1)] = random_x[:k + 1]
  columns3['y{}'.format(k + 1)] = moist[:k + 1]

  columns4['x{}'.format(k + 1)] = random_x[:k + 1]
  columns4['y{}'.format(k + 1)] = temp[:k + 1]



trace1 = go.Scatter(
    x = random_x,
    y = humid,
    name = 'Humidity',
    fill = 'tozeroy',
    fillcolor = 'rgb(0, 200, 200)',
    line = dict(
        color = ('rgb(0, 0, 100)'),
        width = 4)
)

trace2 = go.Scatter(
    x = random_x,
    y = light,
    name = 'Light',
    fill = 'tozeroy',
    fillcolor = '#B8F7D4',
    line = dict(
        color = ('#3A4750'),
        width = 4)
)


trace3 = go.Scatter(
    x = random_x,
    y = moist,
    name = 'Soil Moisture',
    fill = 'tozeroy',
    fillcolor = '#835AF1',
    line = dict(
        color = ('#7FA6EE'),
        width = 4)
)

trace4 = go.Scatter(
    x = random_x,
    y = temp,
    name = 'Temperature',
    fill = 'tozeroy',
    fillcolor = 'rgb(239, 124, 228)',
    line = dict(
        color = ('rgb(196,5,124)'),
        width = 4)
)

axis=dict(ticklen=4,
          mirror=True,
          zeroline=False,
          showline=True,
          autorange=False,
          showgrid=False)

fig = tools.make_subplots(rows=4, cols=1, print_grid=False, subplot_titles=('Humidity vs Time', 'Light vs Time','Soil Moisture vs Time', 'Temperature vs Time'))

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
                                               mode='immediate')])])]

frames=[dict(name = k,
		data=
			[dict(x= columns1['x{}'.format(k + 1)],y= columns1['y{}'.format(k + 1)],),
			dict(x= columns2['x{}'.format(k + 1)],y= columns2['y{}'.format(k + 1)],),
			dict(x= columns3['x{}'.format(k + 1)],y= columns3['y{}'.format(k + 1)],),
			dict(x= columns4['x{}'.format(k + 1)],y= columns4['y{}'.format(k + 1)],)]) 
for k in range(N-1)]


fig.append_trace(trace1, 1, 1)
fig.append_trace(trace2, 2,1)
fig.append_trace(trace3, 3,1)
fig.append_trace(trace4, 4,1)

data = [trace1, trace2, trace3, trace4]

fig['layout']['xaxis1'].update(nticks = 12, tickangle = -45,range= [0,N])
fig['layout']['xaxis2'].update(nticks = 12, tickangle = -45,range= [0,N])
fig['layout']['xaxis3'].update(nticks = 12, tickangle = -45,range= [0,N])
fig['layout']['xaxis4'].update(title='Time', nticks = 12, tickangle = -45,range= [0,N])

fig['layout']['yaxis1'].update(title= 'Relative Percentage', range=[0,40])
fig['layout']['yaxis2'].update(title= 'Lumens', range=[0,1])
fig['layout']['yaxis3'].update(title= '', range=[0,1])
fig['layout']['yaxis4'].update(title= 'Degrees Celsius', range=[0,40])
fig['layout'].update(height=800, width=800, title='Dynamic Data Parameter Plots')
fig['layout'].update(updatemenus = updatemenus)

fig.update(data=data,frames=frames)
py.offline.plot(fig, filename = 'Subplots Dynamic.html')



