#!/usr/bin/python3
import sys
import re
import json
import dash
import time
import multiprocessing
import requests
import plotly.express       as px
import dash_core_components as dcc
import dash_html_components as html
import dash_cytoscape       as cyto
import plotly.graph_objects as go
from   dash.dependencies    import Input, Output
from   argparse             import ArgumentParser
from   threading            import Thread
from   flask                import Flask, request
from   flask_restful        import Resource, Api

################################## GUI-specific code begins #########################################################
# Load the design spec for visualization
def load_design(infile, suppmsg):
  with open(infile, 'r') as inf: j = json.load(inf)
  m   = {f['flowId']:f['message'] for f in j['flows']}
  l   = {f['flowId']:f['label'] for f in j['flows']}
  s   = {x:c['component'] for c in j['topology'] for x in c['outFlows']}
  d   = {x:c['component'] for c in j['topology'] for x in c['inFlows']}
  lvl = {x['cle-label']:x['cle-json']['level'] for x in j['cles']}

  elts = []
  for c in j['topology']: 
    elts.append({'data': {'id': c['component'], 'label': c['component'], 'level':lvl[c['label']]}})
  for x in m:             
    if not m[x] in suppmsg:
      elts.append({'data': {'source': s[x], 'target':d[x], 'label': m[x]}})
      #elts.append({'data': {'source': s[x], 'target':d[x], 'label': m[x]+':'+l[x]}})
  return elts

# REST API where transcript analyzer can push data and where Dashboard can pull data
class Viewdata(Resource):
  data = []
  def get(self):
    return Viewdata.data 
  def post(self):
    try: 
      json_data = request.get_json()
      Viewdata.data.append(json_data)
    except:
      pass
    return {"status": "okay"}, 200

# Run on a separate process so that it doesn't block
def start_server(app, **kwargs):
  def run():
    app.run_server(**kwargs)
  server_process = multiprocessing.Process(target=run)
  server_process.start()

def setup_gui(args):
  server = Flask('pith')
  api    = Api(server)
  app    = dash.Dash(__name__, server=server)
  elts   = load_design(args.file, args.suppress_messages.split(','))
  url    = 'http://localhost:' + str(args.port) + '/data'

  api.add_resource(Viewdata, '/data')

  app.layout = html.Div([
    html.Div([
      html.P("CLOSURE EOP1 Demo Cross-Domain Message Flow Visualizer"),
      html.Div(id='live-update-data', style={'display': 'none'}),
      dcc.Interval(id='interval-component', interval=1*1000, n_intervals=0)
    ]),
    html.Div([dcc.Graph(id="bar-chart")], style={'width':'40%','float':'left','align':'middle'}),
    cyto.Cytoscape( 
      id='cytoscape', 
      elements=elts, 
      layout={'name': 'breadthfirst'},  # breadthfirst, circle, cose, grid, ...
      stylesheet=[
        {'selector': 'node', 'style': { 'label': 'data(label)', 'background-color': 'data(level)'}},
        {'selector': 'edge', 'style': { 'width':5, 'label': 'data(label)', 'curve-style': 'bezier', 'control-point-weight': 0.9, 'target-arrow-shape': 'triangle'}}
      ], 
      style={'width':'700px','height':'600px','float':'right'}
    ),
  ])

  @app.callback(Output('bar-chart', 'figure'), [Input('live-update-data', 'children')]) 
  def update_bar_chart(data):
    j = json.loads(data)
    lj = {}
    lj1 = {}
    lj2 = {}
    try: lj = j[-1]['stats']
    except: pass

    lj1['count']        = lj['count'] if 'count' in lj else 0
    lj1['cross-domain'] = lj['xd']    if 'xd' in lj else 0
    lj1['local']        = lj1['count'] - lj1['cross-domain']    
    lj2['hwredactdet']  = lj['hwredactdet'] if 'hwredactdet' in lj else 0
    lj2['swredactdet']  = lj['swredactdet'] if 'swredactdet' in lj else 0
    lj2['hwredactmp']   = lj['hwredactmp'] if 'hwredactmp' in lj else 0
    lj2['swredactmp']   = lj['swredactmo'] if 'swredactmp' in lj else 0

    lj.pop('sync',None)
    lj.pop('salient',None)
    lj.pop('count',None)
    lj.pop('xd',None)
    lj.pop('hwredactdet',None)
    lj.pop('swredactdet',None)
    lj.pop('hwredactmp',None)
    lj.pop('swredactmp',None)
    fig = go.Figure([
            go.Bar(name='Message counts by type', x=list(lj.keys()), y=list(lj.values()), text=list(lj.values())), 
            go.Bar(name='Aggregate counts', x=list(lj1.keys()), y=list(lj1.values()), text=list(lj1.values())), 
            go.Bar(name='Redaction counts', x=list(lj2.keys()), y=list(lj2.values()), text=list(lj2.values())), 
          ])
    fig.update_yaxes(type='log')
    return fig

  # Pull data from webserver 
  @app.callback(Output('live-update-data', 'children'), Input('interval-component', 'n_intervals'))
  def update_viewdata(n):
    headers = {'Content-type': 'application/json', 'Accept': 'text/plain'}
    r = requests.get(url, headers=headers)
    return json.dumps(r.json())

  start_server(app,port=args.port,debug=True)
  return app

# Push data to webserver
def update_gui(args,m,c,stats):
  headers = {'Content-type': 'application/json', 'Accept': 'text/plain'}
  data = {'msg':m,'tags':c,'stats':stats}
  r = requests.post('http://localhost:' + str(args.port) + '/data', headers=headers, json=data)
  return r.status_code

################################## GUI-specific code ends #########################################################

# Update statistics
def update_stats(count,c,stats):
  for k in c:
    if not k in stats: stats[k] = 0
    stats[k] += 1
  stats['count'] = count

# Regular expression to extract JSON message from transcript
msgre = re.compile('^.*Received (.*$)')

# Generator to read message from stdin 
def next_msg():
  for line in sys.stdin:
    try:
      m = msgre.match(line)
      if m:
        j = json.loads(m.group(1))
        yield j
    except:
      pass

# Infer message type
def classify(loc,rem,case,msg):
  a = {}
  a['unpart']               = True if (loc == rem) else False
  a['xd']                   = True if ('fromRemote' in msg) else False
  a['component_heartbeats'] = True if ('ss' in msg) else False
  a['sync']                 = True if ('ss' in msg) and (msg['ss'] == 'All') else False
  a['pnt']                  = True if ('time' in msg) else False
  a['groundMovers']         = True if ('bearing' in msg) else False
  a['updateMissionPlan']    = True if ('missionPlan' in msg) else False
  a['reqXXXDetections']     = True if ('phase' in msg) else False
  try: 
    a['rcvISRMDetections']  = all(d['classification'] != '' and d['speed'] != -1.0 for d in msg['detects'])
  except: 
    a['rcvISRMDetections']  = False
  try: 
    a['rcvEOIRDetections']  = all(d['classification'] != '' and d['speed'] == -1.0 for d in msg['detects'])
  except: 
    a['rcvEOIRDetections']  = False
  try: 
    a['rcvRDRDetections']   = all(d['classification'] == '' for d in msg['detects'])
  except: 
    a['rcvRDRDetections']   = False
  a['swredactdet']          = all(d['alt'] == -9999.0 for d in msg['detects'] if 'alt' in d) if 'detects' in msg else False
  a['hwredactdet']          = (a['xd'] == True and all(d['alt'] == 0.0 for d in msg['detects'] if 'alt' in d)) if 'detects' in msg else False
  try: 
    a['swredactmp']         = all([w['z'] == -9999.0 for w in msg['missionPlan']['vehiclePlan']['wayPoints']])
  except:
    a['swredactmp']         = False
  try: 
    a['hwredactmp']         = (a['xd'] == True and all([w['z'] == 0.0 for w in msg['missionPlan']['vehiclePlan']['wayPoints']]))
  except:
    a['hwredactmp']         = False
  a['salient']              = a['sync'] or a['updateMissionPlan'] or a['reqXXXDetections'] or a['rcvRDRDetections'] or a['rcvEOIRDetections'] or a['rcvISRMDetections']
  return [i for i in a if a[i] == True]

# Parse command line argumets
def get_args():
  p = ArgumentParser(description='CLOSURE EOP Message Transcript Viewer')
  p.add_argument('-f', '--file', required=False, type=str, default='design_spec.json', help='Input file (design_spec.json)')
  p.add_argument('-s', '--suppress_messages', required=False, type=str, default='component_heartbeats,pnt,groundMovers', help='Comma-separated messages to suppress (component_heartbeats,pnt,groundMovers)')
  p.add_argument('-m', '--mode', required=False, type=str, default='txt', help='Mode can be gui or txt (txt)')
  p.add_argument('-l', '--local_enclave', required=True, type=str, help='Local enclave')
  p.add_argument('-r', '--remote_enclave', required=True, type=str, help='Remote enclave')
  p.add_argument('-c', '--case', required=True, type=str, help='EOP1 MA Demosntration Case)')
  p.add_argument('-b', '--batch_stats', required=False, type=int, default=200, help='Stats batch size (200)')
  p.add_argument('-p', '--port', required=False, type=int, default=11235, help='Dashboard port (11235)')
  return p.parse_args()

# Run the application
if __name__ == '__main__':
  args = get_args()
  if args.mode == 'gui': app = setup_gui(args)
  time.sleep(1)
  count = 0
  stats = {}
  for m in next_msg():
    count += 1
    c = classify(args.local_enclave,args.remote_enclave,args.case,m)
    update_stats(count,c,stats)
    if args.mode == 'txt' and 'salient' in c                                        : print(c, ' : ', m)
    if args.mode == 'txt' and ((count % args.batch_stats) == 0)                     : print('Stats: ', stats)
    if args.mode == 'gui' and ('salient' in c or ((count % args.batch_stats) == 0)) : update_gui(args,m,c,stats)
    continue
  if args.mode == 'txt' : print('Final Stats: ', stats)
