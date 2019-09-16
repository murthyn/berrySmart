import plotly.graph_objects as go
import plotly

import requests
import numpy as np

def request_handler(request):

    URL = "http://608dev.net/sandbox/sc/garciag/all_dataHandler.py"
    r = requests.get(url=URL)
    data = eval(r.text)

    temp1 = data[0]
    temp2 = data[1]
    temp3 = data[2]
    temp4 = data[3]

    humid1 = data[4]
    humid2 = data[5]
    humid3 = data[6]
    humid4 = data[7]

    light1 = data[8]
    light2 = data[9]
    light3 = data[10]
    light4 = data[11]

    moist1 = data[12]
    moist2 = data[13]
    moist3 = data[14]
    moist4 = data[15]

    N = len(data)
    random_x = np.linspace(0, 168,N)

    # Initialize figure
    fig = go.Figure()

    # Create traces

    #Temperature 
    fig.add_trace(go.Scatter(
        x = random_x,
        y = temp1,
        name = 'Node 1',
        mode = 'lines+markers'
    ))

    fig.add_trace(go.Scatter(
        x = random_x,
        y = temp2,
        name = 'Node 2',
        mode = 'lines+markers'
    ))

    fig.add_trace(go.Scatter(
        x = random_x,
        y = temp3,
        name = 'Node 3',
        mode = 'lines+markers'
    ))

    #Humidity
    fig.add_trace(go.Scatter(
        x = random_x,
        y = temp4,
        name = 'Node 4',
        mode = 'lines+markers'
    ))

    fig.add_trace(go.Scatter(
        x = random_x,
        y = humid1,
        name = 'Node 1',
        mode = 'lines+markers'
    ))

    fig.add_trace(go.Scatter(
        x = random_x,
        y = humid2,
        name = 'Node 2',
        mode = 'lines+markers'
    ))

    fig.add_trace(go.Scatter(
        x = random_x,
        y = humid3,
        name = 'Node 3',
        mode = 'lines+markers'
    ))

    fig.add_trace(go.Scatter(
        x = random_x,
        y = humid4,
        name = 'Node 4',
        mode = 'lines+markers'
    ))

    #Light 
    fig.add_trace(go.Scatter(
        x = random_x,
        y = light1,
        name = 'Node 1',
        mode = 'lines+markers'
    ))

    fig.add_trace(go.Scatter(
        x = random_x,
        y = light2,
        name = 'Node 2',
        mode = 'lines+markers'
    ))

    fig.add_trace(go.Scatter(
        x = random_x,
        y = light3,
        name = 'Node 3',
        mode = 'lines+markers'
    ))

    fig.add_trace(go.Scatter(
        x = random_x,
        y = light4,
        name = 'Node 4',
        mode = 'lines+markers'
    ))
    #Soil Moisture
    fig.add_trace(go.Scatter(
        x = random_x,
        y = moist1,
        name = 'Node 1',
        mode = 'lines+markers'
    ))

    fig.add_trace(go.Scatter(
        x = random_x,
        y = moist2,
        name = 'Node 2',
        mode = 'lines+markers'
    ))

    fig.add_trace(go.Scatter(
        x = random_x,
        y = moist3,
        name = 'Node 3',
        mode = 'lines+markers'
    ))

    fig.add_trace(go.Scatter(
        x = random_x,
        y = moist4,
        name = 'Node 4',
        mode = 'lines+markers'
    ))

    fig.update_layout(
        updatemenus=[
            go.layout.Updatemenu(
                active=0,
                buttons=list([
                    dict(label="All",
                         method="update",
                         args=[{"visible": [True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True]},
                               {"title": "Entire Farm",
                               "xaxis" : dict(title = 'Time (hours)', autorange = 'reversed'),
                               "yaxis" : dict(title = '')}]),
                    dict(label="Temperature",
                         method="update",
                         args=[{"visible": [True,True,True,True,False,False,False,False,False,False,False,False,False,False,False,False]},
                               {"title": "Farm Temperature",
                               "xaxis" : dict(title = 'Time (hours)',autorange = 'reversed'),
                               "yaxis" : dict(title = 'Degrees Celsius')}]),
                    dict(label="Humidity",
                         method="update",
                         args=[{"visible": [False,False,False,False,True,True,True,True,False,False,False,False,False,False,False,False]},
                               {"title": "Farm Light",
                               "xaxis" : dict(title = 'Time (hours)', autorange = 'reversed'),
                               "yaxis" : dict(title = 'Percentage')}]),
                    dict(label="Light",
                         method="update",
                         args=[{"visible": [False,False,False,False,False,False,False,False,True,True,True,True,False,False,False,False]},
                               {"title": "Farm Light",
                               "xaxis" : dict(title = 'Time (hours)',autorange = 'reversed'),
                               "yaxis" : dict(title = 'Normalised Amperes')}]),
                    dict(label="Soil Moisture",
                         method="update",
                         args=[{"visible": [False,False,False,False,False,False,False,False,False,False,False,False,True,True,True,True]},
                               {"title": "Farm Soil Moisture",
                               "xaxis" : dict(title = 'Time (hours)',autorange = 'reversed'),
                               "yaxis" : dict(title = 'Percentage')}]),
                ]),
            )
        ])

    # Set title
    fig.update_layout(title_text="Entire Farm",xaxis = dict(title = 'Time (hours)'))
    fig.update_xaxes(autorange="reversed")
    #fig.show()
    return plotly.offline.plot(fig,output_type='div')


