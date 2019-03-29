from math import pow, sqrt
from threading import Thread, Lock
from queue import Queue
from traffic_simulation import *
from socket import *
from time import sleep

import json

class Worker(Thread):
	def __init__(self, sock, peer, wid):
		self.id = wid
		
		self.connSocket = sock
		self.peer = peer

		self.__terminate = False
		self.simulation = Simulation() # Creating our simulation for the user
		self.mp = None
		self.currentResponseType = [0 for i in range(6)]

		self.tickMutex = Lock()
		self.tickMutex.acquire()
		
		self.sendingMutex = Lock()
		
		self.carEnExQueue = Queue()
		self.carStFnQueue = Queue()
		
		self.simulation.ticker.setWorkerThread(self)
		
		self.idle = True
		
		self.finishedCars = []
		
		super().__init__()
		
	def reportCarInsertion(self,vh,rs):
		indvStat = self.getStatForVehicle(vh,rs)
		self.carEnExQueue.put(indvStat)
	
	def reportCarCreated(self,vh,rs):	
		indvStat = self.getStatForVehicle(vh,rs)
		self.carStFnQueue.put(indvStat)
	
	def reportCarFinished(self,vh,rs):
		indvStat = self.getStatForVehicle(vh,rs)
		self.finishedCars.append( indvStat ) ## bad design
		self.carStFnQueue.put(indvStat)
		self.carEnExQueue.put(indvStat)
			

	def getStatForVehicle( self, vh, rs ):
		if rs == None:
			passedTime = self.simulation.ticker.passedTicks - vh.sTick
			passedTime = passedTime if not passedTime == 0 else 1
			endVertex = self.simulation.mmap.getNode( vh.dest )
			#print("------------------->",tuple(( [vh.source]+self.simulation.mmap.getShortestpath(vh.source,vh.dest)[1])))
			return ( vh.id, vh.source, vh.dest, tuple(( [vh.source]+self.simulation.mmap.getShortestpath(vh.source,vh.dest)[1])[-2:]),\
				vh.total, vh.remDist, passedTime, vh.total/passedTime, None, ( endVertex.x, endVertex.y )  )
		elif vh.path == []:
			u = vh.currRs.edge.start
			v = vh.currRs.edge.end
			rsTotalDist = vh.currRs.edge.dist
			rsPassedDist = rsTotalDist - vh.remDist
			k = rsPassedDist / rsTotalDist
			passedTime = self.simulation.ticker.passedTicks - vh.sTick
			
			passedTime = passedTime if not passedTime == 0 else 1
			endVertex = self.simulation.mmap.getNode( vh.dest )
			#print("------------------->",tuple(( [vh.source]+self.simulation.mmap.getShortestpath(vh.source,vh.dest)[1])))
			return ( vh.id, vh.source, vh.dest, tuple(( [vh.source]+self.simulation.mmap.getShortestpath(vh.source,vh.dest)[1])[-2:]),\
				vh.total, vh.remDist, passedTime, (vh.total-vh.remDist )/passedTime, vh.currSpeed, ( (u.x + (v.x-u.x)*k, u.y + (v.y-u.y)*k) )  )
		else:
			vn, vf = vh.getFirstAndLastPath()
			# vid, source, dest,(currs.start, currs.end), totalKm, remKm
			# passedTime, avgSpeed, curSpeed, coor
		
			u = rs.edge.start
			v = rs.edge.end
			rsTotalDist = rs.edge.dist
			rsPassedDist = rsTotalDist - vh.remDist
			k = rsPassedDist / rsTotalDist
			passedTime = self.simulation.ticker.passedTicks - vh.sTick
			#print("DBG 1:",rsPassedDist)
			#print("DBG 3:",vh.path,vn,vf)
			#print("DBG 2:",self.simulation.mmap.getShortestpath(vn,vf))
			avgSpeed = (rsPassedDist + self.simulation.mmap.getShortestpath(vn,vf)[0]) / ( passedTime if not passedTime == 0 else float('inf') )

			return (vh.id, vh.source, vh.dest,
					(u.vid, v.vid), vh.total, vh.remDist+self.simulation.mmap.getShortestpath(vn,vf)[0],
					passedTime, avgSpeed, vh.currSpeed, (u.x + (v.x-u.x)*k, u.y + (v.y-u.y)*k)) ## last is curr coords


	def resCarStat(self):
		mmap = self.simulation.getMap()
		allCars = []
		for rs in mmap.getRsegments(): ## TODO BERBAT
			while rs.idle == False:
				pass
		for rs in mmap.getRsegments():
			for vh in rs.vehicles:
				allCars.append( self.getStatForVehicle(vh,rs) )
		return allCars
		
	def resCarEnterExit(self):
		carResps = []
		while not self.carEnExQueue.empty():
			carResps.append( self.carEnExQueue.get(False) )
		return carResps
	
	def resCarStartFinish(self):
		carResps = []
		while not self.carStFnQueue.empty():
			carResps.append( self.carStFnQueue.get(False) )
		return carResps

	def resEdgeStats(self):
		mmap = self.simulation.getMap()
		allStats = []
		for rs in mmap.getRsegments():
			allStats.append( [ (rs.edge.start.vid,rs.edge.start.x, rs.edge.start.y),\
							(rs.edge.end.vid, rs.edge.end.x, rs.edge.end.y), len(rs.vehicles)\
							,rs.capacity,rs.currWorstSpeed,rs.average ] ) 
		return allStats
		
	def resSimStats(self):
		return self.simulation.getStats()
	
	def resSimReport(self):
		rp = [0 for i in range(3)]
		rp[0] = self.finishedCars
		rp[1] = self.resEdgeStats()
		rp[2] = self.resSimStats()
		#print(rp)
		return rp
		
	def sendUsingProtocol(self,dataList):
		with self.sendingMutex:
			dumpedByteArr = json.dumps(dataList).encode()
			#print( "Sending :", len(dumpedByteArr) )
			self.connSocket.send( ('{:10d}'.format(len(dumpedByteArr))).encode() )
			#print( "Sending :", dumpedByteArr )
			self.connSocket.send( dumpedByteArr )
			print("WTSocket : Data send to",self.peer, ", len=",len(dumpedByteArr))
	
	def receive(self, Continiously = True):
		while Continiously and not self.__terminate:
			sendMapInfo = True
			rqLen = self.connSocket.recv(10)
			rq = self.connSocket.recv(int(rqLen))
			rqObject = json.loads(rq.decode())
			
			try:
				rqObject = eval(rqObject) ## TODO wow
			except Exception as e:
				print("Unknown command4", rqObject, "E :", e)
				sendMapInfo = False
			## DECODE THE COMMAND GIVEN
			if not type( rqObject ) == list:
				print("Unknown command5", rqObject)
				sendMapInfo = False
			elif len( rqObject ) == 2: # add or remove operations
				if "debug_level" in rqObject[0]:
					self.currentResponseType = rqObject[1]
					sendMapInfo = False
				elif "add_nodes" in rqObject[0]:
					for n in rqObject[1]:
						self.mp.addNode( n[0],n[1],n[2] )
				elif "add_edges" in rqObject[0]:
					for r in rqObject[1]:
						if len(r) == 4:
							self.mp.addRoad( r[0],r[1],r[2],r[3] )
						elif len(r) == 3:
							self.mp.addRoad( r[0],r[1],r[2] )
						else:
							print("Unknown command7", rqObject)
							sendMapInfo = False
				elif "add_generators" in rqObject[0]:
					for g in rqObject[1]:
						self.simulation.addGenerator( g[0],g[1],g[2],g[3] )
				elif "start_simulation" in rqObject[0]:
					self.simulation.startSimulation( int(rqObject[1]), self )
					
					t2 = Thread( target = self.waitAndTerminate ) ## TODO make it self
					t2.start()
					sendMapInfo = False
				elif "set_map" in rqObject[0]:
					self.mp = Map(rqObject[1][0],rqObject[1][1] )
					self.simulation.setMap( self.mp )
				elif "save_map" in rqObject[0]:
					self.mp.saveMap( rqObject[1][0],rqObject[1][1] ) # username, mapname
					self.simulation.setMap( self.mp )
				elif "load_map" in rqObject[0]:
					self.mp.loadMap( rqObject[1][0],rqObject[1][1] ) # username, mapname
					self.simulation.setMap( self.mp )
				elif "delete_map" in rqObject[0]:
					self.mp.deleteMap( rqObject[1][0],rqObject[1][1] ) # username, mapname
				else:
					print("Unknown command1", rqObject)
			elif len( rqObject ) == 1: # other operations
				parsedStr = rqObject[0]
				if "end_simulation" in parsedStr:
					self.currentResponseType = "END"
					sendMapInfo = False
				elif "tick" in parsedStr:
					self.simulation.tick()
					sendMapInfo = False
				else:
					print("Unknown command2", rqObject)
					sendMapInfo = False
			else:
				print("Unknown command3", rqObject)
				sendMapInfo = False
			
			if sendMapInfo:
				self.sendUsingProtocol([[],[],[],self.resEdgeStats(),self.resSimStats(),[]])
			
	def waitAndTerminate(self):
		self.simulation.wait()
		self.simulation.terminate()
		print( "WorkerT : Terminate Signal send by simulation to", self.id , ", it has ended" )
		self.sendUsingProtocol( [[],[],[],[],[], self.resSimReport() ] )
		print("WorkerT : Client", self.peer, "is disconnected, last tick was no:",self.simulation.ticker.passedTicks)
		

	def Terminate(self):
		self.__terminate = True

	def run(self):
		#self.receive(False)   # First recive without thread since we dont know what the user wants 
		t1 = Thread( target = self.receive)
		t1.start()
		
		self.tickMutex.acquire()
		print( "Worker", self.id," started simulation" )
		while not self.__terminate:
			self.idle = True
			self.tickMutex.acquire()
			self.idle = False
			if self.simulation.ticker.passedTicks % 10 == 0:
				print( "Worker, ",self.id,"tick -> ",self.simulation.ticker.passedTicks )
			if self.currentResponseType == "END":
				self.__terminate = True
				break

			response = [ [] for i in range(6)]
			
			
			# CarStat
			if self.currentResponseType[0]:
				response[0] = self.resCarStat()
				
			# CarEnterExit
			if self.currentResponseType[1]:
				response[1] = self.resCarEnterExit()
				
			# CarStarFinish
			if self.currentResponseType[2]:
				response[2] = self.resCarStartFinish()
			
			# EdgeStat
			if self.currentResponseType[3]:
				response[3] = self.resEdgeStats()
			
			# SimStat
			if self.currentResponseType[4]:
				response[4] = self.resSimStats()
			
			#Send the data
			self.sendUsingProtocol(response)
		
		#SimReport
				
				

class Server(Thread):
	def __init__(self):
		self.clients = []  
		
		self.port = int(input("On port :"))#20445
		
		super().__init__()
		

	def run(self):
		s = socket(AF_INET, SOCK_STREAM)
		s.bind(('',self.port))
		s.listen(1)
		try:
			while True:
				print("Server is listening on port", self.port)
				connSocket, peer = s.accept()
				print(peer, "connected")
				
				w = Worker(connSocket,peer,len(self.clients))
				w.start()
				self.clients.append(w)
				
		finally:
			s.close()
			
		
			
			
if __name__ == '__main__':
	s = Server()
	s.start()
	
	s.join()			
	
	print("Terminated probably because of wrongly given port number")			
			
	
