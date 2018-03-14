import json
from django.http import HttpResponse
from django.shortcuts import render, redirect
from django.views import View
from time import sleep
from threading import Thread
from tsclient import Client as tsClient
from django.contrib.auth import authenticate,login,logout
from django.contrib.auth.decorators import login_required
from django.contrib.auth.forms import UserCreationForm
from django.http import JsonResponse
import sqlite3
import unicodedata
import random

def hellowWithParam(request):
		return render(request, "index.html")

def signup(request):
	if request.method == 'POST':
		form = UserCreationForm(request.POST)
		if form.is_valid():
			form.save()
			request.session['username'] = request.POST['username']
			return redirect('/login')
	else:
		form = UserCreationForm()
	return render(request, 'signup.html', {'form': form})

def login_post(request):
	'''Login to system'''
	# check for authentication
	if 'username' in request.POST and 'password' in request.POST:
		# create login session 
		username = request.POST['username']
		password = request.POST['password']
		user = authenticate(username=username, password=password)
		if user:
			login(request, user)
			request.session['username'] = username
			return redirect('/trafficserver')
		else:
			# Return an 'invalid login' error message.
			return render(request, 'login.html', {'message': 'Invalid username or password'})
	# first time
	else:
		'''Show login form'''
		context =  {'message': 'Welcome. Login or create an account!'}
		return render(request, 'login.html', context)

def logout_view(request):
	'''Simply logout'''
	logout(request)
	return redirect('/trafficserver')


def singleton(class_):
	instances = {}
	def getinstance(*args, **kwargs):
		if class_ not in instances:
			instances[class_] = class_(*args, **kwargs)
		return instances[class_]
	return getinstance
			
@singleton
class proxy:
	def __init__(self, port=2566):
		if not hasattr(self,'__clientDict'):
			self.__clientDict = {}
			self.port = port
			super().__init__()

	def getClient(self, uid):
		print( self.__clientDict )
		if not uid in self.__clientDict:
			try:
				self.__clientDict[uid] = tsClient('', self.port)
				self.__clientDict[uid].start()
				self.__clientDict[uid].sendUsingProtocol( "['set_map',['" + uid + "','" + str( random.randint(0,10000000) ) + "']]" )
			except:
				return "Connection Error"
			print( self.__clientDict )
		
		return self.__clientDict[uid]
	
	def deleteClient( self,uid ):
		if uid in self.__clientDict:
			del self.__clientDict[uid]
	
	def sendDataWithClient( self, cl, data ):
		try:
			cl.sendUsingProtocol(str(data))
		except:
			pass
	
	def listMaps(self,cl,username):
		with sqlite3.connect("traffic_simulation.sqlite3") as db:
			cur = db.cursor()
			cur.execute("SELECT user,map_name FROM maps WHERE user=?", (username,))
			
			rows = cur.fetchall()
			ret = []
			for row in rows:
				ret.append( ' - '.join(row) )
			return ret
	
	def getCurrentStateOfClient( self,cl ):
		ll = cl.getCurrentState()
		return ll
	
	def getNodesFromClient( self, cl ):
		nodes = {}
		try:
			nodes1 = {linst[0][0]:[ linst[0][1],linst[0][2] ] for linst in cl.getCurrentState()[3]}
			nodes2 = {linst[1][0]:[ linst[1][1],linst[1][2] ] for linst in cl.getCurrentState()[3]}
			if len(nodes1)+len(nodes2) == 0:
				try:
					nodes1 = {linst[0][0]:[ linst[0][1],linst[0][2] ] for linst in cl.getCurrentState()[5][1]}
					nodes2 = {linst[1][0]:[ linst[1][1],linst[1][2] ] for linst in cl.getCurrentState()[5][1]}
					nodes = { **nodes1,**nodes2 }
					return nodes
				except:
					pass
			nodes = { **nodes1,**nodes2 }
		except: 
			try:
				nodes1 = {linst[0][0]:[ linst[0][1],linst[0][2] ] for linst in cl.getCurrentState()[5][1]}
				nodes2 = {linst[1][0]:[ linst[1][1],linst[1][2] ] for linst in cl.getCurrentState()[5][1]}
				nodes = { **nodes1,**nodes2 }
			except:
				pass
			pass
		return nodes
	
	def getEdgesFromClient( self, cl ):
		edges = []
		try:
			#  0 0 -> svid
			#  1 0 -> evid
			#  2 -> numofcars
			#  3 -> capacity
			edges = [[linst[0][0],linst[1][0],(linst[2]/linst[3])] for linst in cl.getCurrentState()[3]]
			if len(edges) == 0:
				try:
					edges = [[linst[0][0],linst[1][0],(linst[2]/linst[3])] for linst in cl.getCurrentState()[5][1]]
					return edges
				except:
					pass
		except:
			try:
				edges = [[linst[0][0],linst[1][0],(linst[2]/linst[3])] for linst in cl.getCurrentState()[5][1]]
			except:
				pass
			pass
		return edges
	
	def getVehiclesFromClient( self, cl ):
		vehicles = []
		try:
			vehicles = [[linst[0],linst[9][0],linst[9][1]] for linst in cl.getCurrentState()[0]]
		except:
			pass
		return vehicles
	
	def getGeneratorsFromClient( self, cl ):
		geners = []
		try:
			geners = cl.getCurrentState()[4][5] ## line changed
		except:
			pass
		return geners
	def getTickCountFromClient( self, cl ):
		tc = 0
		try:
			tc = int( cl.getCurrentState()[4][6] ) ## line changed
		except:
			pass
		return tc
	
	def getStateOfSim( self, cl ):
		return 1 if len( cl.getCurrentState()[5] ) != 0 else 0 
		
						
class userView(View):
	def __init__(self):
		self.__proxy = proxy()
		super().__init__()
		
	def get(self,request):
		#print("GET",request.GET.dict())
		
		points = {}
		edges = []
		vehicles = []
		geners = []
		reports = []
		map_list = []
		sim_state = 1
		username = ''
		map_name = ''
		obs_mode = 0
		edit_mode = 0
		tc = 0
		
		if 'log_out' in request.GET.dict():
			request.COOKIES.clear()
			request.session.clear()
			context =  {'message': 'Welcome. Login or create an account!'}
			return render(request, 'login.html', context)

		
		if 'o_mode' in request.GET.dict():
			request.session['editing_mode'] = 0
			request.session['observe_mode'] = 1
		
		if 'e_mode' in request.GET.dict():
			request.session['editing_mode'] = 1
			request.session['observe_mode'] = 0
		
		if 'username' in request.session:
			username = request.session.get('username','')
			
			
			cl = self.__proxy.getClient(username)
			if type(cl) == str:	  
				return render(request, "error.html" )
			if 'tick' in request.GET.dict():
				self.__proxy.sendDataWithClient( cl,"['tick']" )
				
			reports = self.__proxy.getCurrentStateOfClient( cl )
			points = self.__proxy.getNodesFromClient( cl )
			edges = self.__proxy.getEdgesFromClient( cl )
			geners = self.__proxy.getGeneratorsFromClient( cl )
			vehicles = self.__proxy.getVehiclesFromClient( cl )
			tc = self.__proxy.getTickCountFromClient( cl )
			obs_mode = request.session.get('observe_mode',0)
			edit_mode = request.session.get('editing_mode',1)
			map_list = self.__proxy.listMaps(cl,username)
			sim_state = self.__proxy.getStateOfSim(cl)
		
		
		render_args = {**{ 'username':request.session.get('username',''), 'map_list' : map_list,\
							'points':points, 'edges': edges, 'vehicles':vehicles,\
							'geners':geners, 'tc':tc, 'observe_mode' : obs_mode, 'editing_mode' : edit_mode,\
							'reports': reports, 'sim_state': sim_state },\
								**request.GET.dict()}
		
		return render(request, "index.html", render_args )
	
	def post(self,request):
		#print("POST",request.POST.dict())
		
		points = {}
		edges = []
		vehicles = []
		map_list = []
		geners = []
		username = ''
		reports = []
		sim_state = 1
		obs_mode = 0
		edit_mode = 0
		tc = 0
		
		if not "username" in request.session:
			request.session['username'] = request.POST.get('username','')
		
		if 'username' in request.session:
			username = request.session.get('username','')
			
			if 'o_mode' in request.POST.dict():
				request.session['editing_mode'] = 0
				request.session['observe_mode'] = 1
		
			if 'e_mode' in request.POST.dict():
				request.session['editing_mode'] = 1
				request.session['observe_mode'] = 0
			
			cl = self.__proxy.getClient(username)
			if type(cl) == str:	  
				return render(request, "error.html" )
			
			self.parseDataFromUserForms( request, cl, username )
			
			reports = self.__proxy.getCurrentStateOfClient( cl )
			points = self.__proxy.getNodesFromClient( cl )
			edges = self.__proxy.getEdgesFromClient( cl )
			geners = self.__proxy.getGeneratorsFromClient( cl )
			vehicles = self.__proxy.getVehiclesFromClient( cl )
			tc = self.__proxy.getTickCountFromClient( cl )
			obs_mode = request.session.get('observe_mode',0)
			edit_mode = request.session.get('editing_mode',1)
			map_list = self.__proxy.listMaps(cl,username)
			sim_state = self.__proxy.getStateOfSim(cl)
		
		render_args = {**{ 'username':request.session.get('username',''),\
			'map_list' : map_list, 'points':points, 'edges': edges,\
			'vehicles':vehicles, 'geners':geners, 'tc':tc,\
			'observe_mode' : obs_mode, 'editing_mode' : edit_mode,\
			'reports': reports, 'sim_state' : sim_state }, **request.POST.dict()}
		
		print("-----> SENDING RESPONSE " )
		if not "ajax" in request.POST.dict():
			return render(request, "index.html", render_args )
		print("*********AS JSON*********\n")
		
		# ----------------------      DECIDE WHAT TO RETURN 
		
		if "add_nodes" in request.POST.dict():
			return JsonResponse({ 'username':request.session.get('username',''),\
				'points':points }, safe=False)
		
		if "add_edges" in request.POST.dict():
			return JsonResponse({ 'username':request.session.get('username',''),\
				'points':points, 'edges': edges }, safe=False)
		
		if "add_generators" in request.POST.dict():
			return JsonResponse({ 'username':request.session.get('username',''),\
				'geners':geners }, safe=False)
		
		if "start_sim" in request.POST.dict():
			return JsonResponse({ 'username':request.session.get('username',''),\
				'points':points, 'edges': edges,\
				'vehicles':vehicles, 'geners':geners, 'tc':tc,\
				'reports': reports, 'sim_state' : sim_state }, safe=False)
			
		if "save_map" in request.POST.dict():
			return JsonResponse({ 'username':request.session.get('username',''),\
				'map_list' : map_list }, safe=False)
		
		if "load_map" in request.POST.dict():
			return JsonResponse({ 'username':request.session.get('username',''),\
				'map_list' : map_list, 'points':points, 'edges': edges }, safe=False)
		
		if "delete_map" in request.POST.dict():
			return JsonResponse({ 'username':request.session.get('username',''),\
				'map_list' : map_list }, safe=False)
		
		if "restart_conn" in request.POST.dict():
			return JsonResponse({ 'username':request.session.get('username',''),\
				'observe_mode' : obs_mode, 'editing_mode' : edit_mode }, safe=False)
		
		if 'tick' in request.POST.dict():
				return JsonResponse({ 'username':request.session.get('username',''),\
				'map_list' : map_list, 'points':points, 'edges': edges,\
				'vehicles':vehicles, 'geners':geners, 'tc':tc,\
				'reports': reports, 'sim_state' : sim_state }, safe=False)
		
		return JsonResponse({ 'username':request.session.get('username',''),\
			'map_list' : map_list, 'points':points, 'edges': edges,\
			'vehicles':vehicles, 'geners':geners, 'tc':tc,\
			'observe_mode' : obs_mode, 'editing_mode' : edit_mode,\
			'reports': reports, 'sim_state' : sim_state }, safe=False)

	
	def str2bool(self,exp):
		return exp.lower() in ("yes", "true", "t", "1")
	
	def parseDataFromUserForms(self, request, cl, username):
		if "command" in request.POST.dict():
			self.__proxy.sendDataWithClient( cl, request.POST.dict()['command'] )
		
		if "add_nodes" in request.POST.dict():
			rawids = [ int(d) for d in request.POST.dict()['ids'].split(',') ]
			rawx = [ int(d) for d in request.POST.dict()['xs'].split(',') ]
			rawy = [ int(d) for d in request.POST.dict()['ys'].split(',') ]
			zipped = [ list(a) for a in zip( rawids,rawx,rawy ) ]
			data = ['add_nodes',zipped]
			
			self.__proxy.sendDataWithClient( cl, data )
		
		if "add_edges" in request.POST.dict():
			fromN = [ int(d) for d in request.POST.dict()['fromN'].split(',') ]
			toN = [ int(d) for d in request.POST.dict()['toN'].split(',') ]
			lanes = [ int(d) for d in request.POST.dict()['lanes'].split(',') ]
			bidir = [ self.str2bool(d) for d in request.POST.dict()['bidir'].split(',') ]
			zipped = [ list(a) for a in zip( fromN,toN,lanes,bidir ) ]
			data = ['add_edges',zipped]
			
			self.__proxy.sendDataWithClient( cl, data )
		
		if "add_generators" in request.POST.dict():
			fromL = [ int(d) for d in request.POST.dict()['fromL'].split(',') ]
			toL = [ int(d) for d in request.POST.dict()['toL'].split(',') ]
			interval = int(request.POST.dict()['interval'])
			tbPushed = int(request.POST.dict()['tbPushed'])
			zipped = [ fromL,toL,interval,tbPushed ]
			data = ['add_generators',[zipped]]
			
			self.__proxy.sendDataWithClient( cl, data )
		
		if "start_sim" in request.POST.dict():
			debugLevel = [ int(request.POST.get('d1',0)),int(request.POST.get('d2',0)),\
						int(request.POST.get('d3',0)),int(request.POST.get('d4',0)),\
						int(request.POST.get('d5',0)),1]
			data = ['debug_level',debugLevel]
			self.__proxy.sendDataWithClient( cl, data )
			
			data = ['start_simulation',int(request.POST.get('tInterval',0))]
			self.__proxy.sendDataWithClient( cl, data )
			
			request.session['editing_mode'] = 0
			request.session['observe_mode'] = 1
			
		if "save_map" in request.POST.dict():
			map_name = request.POST.get('map_name','error')
			data = ['save_map',[ username, map_name ]]
			self.__proxy.sendDataWithClient( cl, data )
		
		if "load_map" in request.POST.dict():
			map_name = request.POST.get('db_maps_load','error')
			data = ['load_map',[ username, map_name.split(" - ")[1] ]]
			self.__proxy.sendDataWithClient( cl, data )
		
		if "delete_map" in request.POST.dict():
			map_name = request.POST.get('db_maps_delete','error')
			data = ['delete_map',[ username, map_name.split(" - ")[1] ]]
			self.__proxy.sendDataWithClient( cl, data )
		
		if "restart_conn" in request.POST.dict():
			self.__proxy.deleteClient( username )
		
		if 'tick' in request.POST.dict():
				self.__proxy.sendDataWithClient( cl,"['tick']" )
		
