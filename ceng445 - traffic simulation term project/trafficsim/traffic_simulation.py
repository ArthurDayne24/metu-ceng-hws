"""

Traffic Simulation Phase 1

Onur Tirtir - 2099380
Mert Tunc - 2099414
"""

from random import randint
from threading import Thread, Lock
from math import sqrt
from time import sleep
import pickle
import sqlite3
import base64


outToStdout = False

class Vertex:
	def __init__(self, vid, x, y):
		self.vid = vid
		self.x = x
		self.y = y

class Edge:
	def __init__(self, start, end):
		self.start = start
		self.end   = end
		self.dist  = sqrt((start.y-end.y)**2 + (start.x-end.x)**2)

class Vehicle:
	def __init__(self, path, cid, source, dest, total, sTick):
		self.sTick	= sTick
		self.cRsTick = sTick
		self.source   = source
		self.dest	 = dest
		self.currRs = None
		self.total	= total
		self.path	 = path
		self.remDist  = None
		self.currSpeed = 0
		self.id = cid
		
		self.mutex = Lock()

	def setRemainingDistance( self, dist ):
		self.remDist = dist
	
	def setCurrentRs( self, currRs ):
		self.currRs = currRs
	
	def setcRsTick( self, cRsTick ):
		self.cRsTick = cRsTick
		
	def setCurrSpeed( self, speed ):
		self.currSpeed = speed
	
	def removePrevFromPath( self ):
		with self.mutex:
			self.path = self.path[1:]
	
	def getFirstAndLastPath(self):
		with self.mutex:
			return (self.path[0],self.path[-1])
	
	def getId(self):
		return self.id

class Generator(Thread):
	gid = 0
	def __init__(self, slis, tlis, per, num, mmap, carcounter, passedTicks):
		self.id = Generator.gid
		Generator.gid += 1

		self.passedTicks = passedTicks

		self.slis = slis
		self.tlis = tlis
		self.per  = per
		self.num  = num
		self.mmap = mmap
		
		self.workerThread = None
		
		self.carcounter = carcounter

		self.mutex = Lock()
		self.mutex.acquire()
		
		self.__terminate = False
		
		super().__init__()
		
	def setWorkerThread(self,workerThread):
		self.workerThread = workerThread
		
	def run(self): 
		if len(self.slis) == len(self.tlis) and len(self.slis) == 1 and self.slis[0] == self.tlis[0]:
			if outToStdout : print(" Illegal start and terminate lists is given ")
			return
		currentPer = self.per
		while self.num > 0 and not self.__terminate:
			while currentPer > 0 and not self.__terminate:
				self.mutex.acquire()
				self.passedTicks += 1
				if outToStdout : print( "Generator {} tickCounter = ".format(self.id),currentPer )
				currentPer-=1
			
			currentPer += 1
			
			s = self.slis[ randint(0, len(self.slis)-1) ]
			d = self.tlis[ randint(0, len(self.tlis)-1) ]
			#print( "Trying to add Vehicle from ",s," to ", d )

			tmp = self.mmap.getShortestpath( s,d )

			if tmp == None or tmp[0] == None or tmp[0] == float('inf') or tmp[1] == []:
				if outToStdout : print('Failed to Add : No such path exists or cycle detected.')
				continue

			#print(tmp)

			spath = tmp[1] ## s ile spath[0] arasi road seg 
			roadToPush = self.mmap.getRoad( s,spath[0] )

			vehicleToPush = Vehicle( spath, self.carcounter.getTotalCarsGenerated(),\
					s, d, tmp[0], self.passedTicks)
			canInsert = roadToPush.insertVehicle( vehicleToPush )
			
			if canInsert:
				self.carcounter.vecPushed()
				if outToStdout : print( "Vehicle (",vehicleToPush.getId(),") is pushed with path : ",([s]+spath) )
				if self.workerThread:
					self.workerThread.reportCarCreated( vehicleToPush, roadToPush)
					

			else:
				if outToStdout : print('Failed to Add : Road is full')
				continue
			
			#for e in self.mmap.edges:
			#	   print( e.edge.start.vid,e.edge.end.vid, e.getNVehicles() )
			
			self.num -= 1
			currentPer = self.per
			
	def Terminate( self ):
		self.__terminate = True

class TickGenerator( Thread ):
	def __init__(self):
		self.geners = []
		self.rsegments = []
		self.tickPer = None
		self.passedTicks = 0

		self.workerThread = None
		self.__terminate = False

		super().__init__()

	def setWorkerThread(self, workerThread):
		self.workerThread = workerThread

	def setTickPeriod( self, tickPer ):
		self.tickPer = tickPer
		
	def Tick(self):
		self.passedTicks += 1
		for t in self.geners:
			if t.isAlive():
				t.mutex.release()
			else:
				self.geners.remove(t)
		for t in self.rsegments:
			if t.isAlive():
				t.tickMutex.release()

		if self.workerThread and self.workerThread.idle:
			self.workerThread.tickMutex.release()
			
	def run(self):
		localtc = 0
		while not self.__terminate:
			sleep( self.tickPer/1000 )
			if outToStdout : print( "\n+++---------------------- TICK ",localtc," ----------------------+++" )
			localtc += 1
			self.Tick()

	def Terminate(self):
		self.__terminate = True
		
		for t in self.geners:
			try:
				t.mutex.release()
			except:
				pass

		for t in self.rsegments:
			try:
				t.tickMutex.release()
			except:
				pass

class CarCounter:
	def __init__(self, mutex):
		self.pushed = 0
		self.popped = 0
		self.doneMutex = mutex

		self.mutex = Lock()

	def vecPushed(self):
		with self.mutex:
			self.pushed += 1

			if self.pushed - self.popped == 1:
				self.doneMutex.acquire()

	def vecPopped(self):
		with self.mutex:
			self.popped += 1

			if self.pushed - self.popped == 0:
				self.doneMutex.release()
				#print("Car counter relased its mutex")
	
	def getactiveCarCount(self):
		return self.pushed - self.popped
	
	def getTotalCarsGenerated(self):
		return self.pushed

class Simulation:
	def __init__(self):
		self.mmap = None
		self.ticker = TickGenerator()
		self.isrsempty = Lock()

		self.carcounter = CarCounter(self.isrsempty)
		
		self.carsToBeGenerated = 0
		
		self.workerThread = None
		
	def setMap(self, mmap):
		self.mmap = mmap

	def getMap(self):
		return self.mmap
	
	def addGenerator(self, slis, tlis, per, num):
		newg = Generator( slis,tlis,per,num,self.mmap, self.carcounter, \
				self.ticker.passedTicks)
		self.carsToBeGenerated += num
		newg.start()
		self.ticker.geners.append(newg)
	
	def startSimulation( self, tickperiod = 0, workerThread = None ):
		self.tickPer = tickperiod
		
		for rs in self.mmap.edges:
			rs.initForSimulation( self.tickPer, self.mmap.edges, self.carcounter )
		self.ticker.setTickPeriod( self.tickPer )
		
		if workerThread:
			self.ticker.setWorkerThread( workerThread )
			self.workerThread = workerThread
			for rs in self.mmap.edges:
				rs.setWorkerThread(workerThread)
			for g in self.ticker.geners:
				g.setWorkerThread(workerThread)
			
		self.ticker.rsegments = self.mmap.edges
		
		self.mmap.calculateShortestPaths()
		
		for rs in self.mmap.edges:
			rs.start()
		if self.tickPer: 

			self.ticker.start()
		print("Simulation Start Call Started All Threads")
		
	def getGenerators():
		return self.ticker.geners
		
	def delGenerators(self, n):
		self.ticker.geners[n].Terminate()
		del self.ticker.geners[n]
		
	def tick(self):
		self.ticker.Tick()
		
	def terminate(self):
		for t in self.ticker.geners:
			if t.isAlive():
				t.Terminate()
		for t in self.mmap.edges:
			if t.isAlive():
				t.Terminate()
		
		if self.workerThread:
			self.workerThread.Terminate()
		
		sleep(0.1)
		self.ticker.Terminate()
		
	
	def wait(self):
		for t in self.ticker.geners:
			print("Waiting thread ",t)
			t.join()

		print('There are no generator threads to wait, sim.join is returning after all cars have arrived')
		self.isrsempty.acquire()
	
	def getStats(self):
		avgSpeed = 0
		avgSpeedF = 0
		for rs in self.mmap.edges:
			avgSpeed += rs.average
			avgSpeedF += rs.finishedAverage
		
		if not len(self.mmap.edges) == 0:
			avgSpeed /= float(len(self.mmap.edges))
			avgSpeedF /= float(len(self.mmap.edges))

		maxVehPerRS = tuple([e.capacity for e in self.mmap.edges])
		curVehPerRS = tuple([len(e.vehicles) for e in self.mmap.edges])
		avgPerRS = tuple([e.average for e in self.mmap.edges])
		
		generInfo = []
		for g in self.ticker.geners :
			generInfo.append( [g.slis, g.tlis, g.per, g.num])
		
		"""return (self.pushed, self.popped,\
				self.carcounter.pushed - self.carcounter.popped,\
				avgSpeed, maxVehPerRS, curVehPerRS, avgPerRS)"""
		return ( sum(curVehPerRS),self.carcounter.popped, self.carsToBeGenerated - self.carcounter.pushed,\
				 avgSpeed, avgSpeedF, generInfo, self.ticker.passedTicks )
		
	

class RSegment(Thread):
	def __init__(self, nlen, nlanes, edge):
		self.nlen   =  nlen
		self.nlanes = nlanes
		self.edge   = edge
		self.vehicles   = []
		self.capacity = int(0.03 * self.nlen * self.nlanes)
		if self.capacity < 1:
			self.capacity = 1
		self.currWorstSpeed = float('inf')
		
		self.finishedAverage = 0.
		self.finishedCount = 0.
		self.average = 0. ## calculated differently but probably correct
		self.done	 = 0.
		
		self.kspeed = 4.0

		self.__terminate = False
		self.tickMutex = Lock()
		self.tickMutex.acquire()
		self.idle = True
		
		self.selfmutex = Lock()
		
		self.tickPeriod = 0
		self.rslist = []
		
		self.tickCount = 0
		
		self.workerThread = None
		
		
		super().__init__()
	  
	def getVehicles(self):
		return self.vehicles

	def fcrowd(self, v, l, d):
		fmin, fmax = 0.05, 1.0
		cmin, cmax = 10, 100
		c = v/(l*d)
		if c <= cmin: return fmax
		elif c < cmax: return fmax - (c - cmin) * (fmax - fmin) / (cmax - cmin)
		else: return fmin
		
	def full(self):
		return len(self.vehicles) >= self.capacity

	def insertVehicle(self, v):
		with self.selfmutex:
			if self.full():
				return False
			v.setRemainingDistance( self.edge.dist )
			v.removePrevFromPath()
			v.setcRsTick(self.tickCount)
			v.setCurrentRs( self )
			self.vehicles.append(v)
			
			if self.workerThread:
				self.workerThread.reportCarInsertion( v, self )

			return True

	def initForSimulation(self, tp, rslist, carcounter):
		self.tickPeriod = tp
		self.rslist = rslist
		
		self.carcounter = carcounter
		
		self.__terminate = False
		
	def setWorkerThread(self, workerThread):
		self.workerThread = workerThread
	
	def run(self):
		while not self.__terminate:
			self.idle = True
			self.tickMutex.acquire()
			self.idle = False
			self.tickCount += 1
			worstOfTickSpeed = float('inf')
			for vind in range(len( self.vehicles )): ## CARE using remaining distance of the road
				currSpeedofTheCar = self.fcrowd( vind, self.nlanes , self.edge.dist  ) * self.kspeed
				
				self.average = ( (self.average*self.done) + currSpeedofTheCar ) \
						/ (self.done+1)
				self.done += 1

				deltadist = currSpeedofTheCar
				
				if worstOfTickSpeed > currSpeedofTheCar:
					worstOfTickSpeed = currSpeedofTheCar
				
				self.vehicles[vind].remDist -= deltadist
				self.vehicles[vind].setCurrSpeed(currSpeedofTheCar)
				
				if self.vehicles[vind].remDist <= 0:
					self.vehicles[vind].setCurrSpeed(0)
					self.vehicles[vind].remDist = 0
			
			self.currWorstSpeed = worstOfTickSpeed
			with self.selfmutex:
				vind = 0
				while vind < len(self.vehicles):
					if self.vehicles[vind].remDist <= 0:
						if len(self.vehicles[vind].path) != 0:
							rsToPush = next(filter(lambda rs: rs.edge.start.vid == self.edge.end.vid\
								and rs.edge.end.vid == self.vehicles[vind].path[0], self.rslist ))
							if rsToPush.insertVehicle( self.vehicles[vind]  ) == True:
								if outToStdout : print("Vehicle(",self.vehicles[vind].getId(),") passes from Rsegment\ (", \
									self.edge.start.vid, "-", self.edge.end.vid \
									,") to (", rsToPush.edge.start.vid,"-",rsToPush.edge.end.vid, ")\nwith remaining path" \
									,self.vehicles[vind].path )
								del self.vehicles[vind]
								vind -= 1

							else:
								if outToStdout : print( "Road is full, waiting for next tick" )
								pass

						else:
							if outToStdout : print("Vehicle(",self.vehicles[vind].getId(),") arrived destination !" )
							
							self.finishedAverage = (self.finishedCount*self.finishedAverage + \
								self.edge.dist/( self.tickCount - self.vehicles[vind].cRsTick ))/(self.finishedCount+1)
							self.finishedCount += 1
							
							if self.workerThread:
								self.workerThread.reportCarFinished(self.vehicles[vind],None)
							
							del self.vehicles[vind]
							vind -= 1
							
							self.carcounter.vecPopped()

					vind += 1
		
	def Terminate( self ):
		self.__terminate = True

	def getInfo(self):
		return (self.nlen, self.nlanes, (self.edge.start, self.edge.end))

	def getStats(self):
		return (self.capacity, self.average, len(self.vehicles))

	def getNVehicles(self):
		return len(self.vehicles)

	def getCapacity(self):
		return self.capacity

	def getNodeIds(self):
		return (self.edge.start, self.edge.end)
	
class Map:
	def __init__( self, username, mapname ):
		self.edges = [] # edges
		self.vertices = []
		self.connectionMatrix = [[]]
		self.v2Ind = dict()
		self.__username = username
		self.__mapname = mapname
		self.__isChanged = True
		self.ispathschanged = False
		
		self.edgesToSave = []

	def getConnMatrix(self):
		return self.connectionMatrix

	def getRsegments(self):
		return self.edges

	def addNode( self, nid, nx, ny ):
		if nid not in self.v2Ind:
			self.vertices.append( Vertex(nid, nx, ny) )
			self.__isChanged = True
			self.ispathschanged = True
	
	def deleteNode( self, nid ): # also delete roads that is connected
		del self.vertices[ self.v2Ind[nid] ]
		self.edges = list( filter(lambda e: e.start != nid and e.end != nid, self.edges) )
		self.__isChanged = True
		self.ispathschanged = True
	
	def addRoad( self, id1, id2, nlanes = 1 , bidir = True ):
		try:
			edgeToPush = Edge( next( filter(lambda v: v.vid == id1, self.vertices) ),\
							   next( filter(lambda v: v.vid == id2, self.vertices) ) )
		except Exception as e:
			print('No such edge to connect.', id1,id2)
			return

		self.edges.append( RSegment(edgeToPush.dist,nlanes, edgeToPush) )

		# what is the nlanes if the road is bidirectional,
		# is there a to RSeg or one, what is cap ?
		if bidir == True: 
			edgeToPush2 = Edge( edgeToPush.end, edgeToPush.start )
			self.edges.append( RSegment(edgeToPush2.dist,nlanes, edgeToPush2) )
		
		self.__isChanged = True
		self.ispathschanged = True
	
	def getNode(self, id1 ):
		return next(filter(lambda v: v.vid == id1, self.vertices ))
	
	def getRoad(self, id1, id2 ):
		return next(filter(lambda rs: rs.edge.start.vid == id1\
				and rs.edge.end.vid == id2, self.edges ))
		
	def deleteRoad( self, id1, id2, bidir = 1 ):
		self.edges.remove( next( e for e in self.edges if e.getNodeIds() == (id1,id2) ) )
		if bidir == 1:
			self.edges.remove( next( e for e in self.edges if e.getNodeIds() == (id2,id1) ) )
		self.__isChanged = True
		self.ispathschanged = True
		
	def getShortestpath( self, id1, id2 ):
		if self.ispathschanged:
			self.calculateShortestPaths()
		if self.connectionMatrix[ self.v2Ind[id1] ][ self.v2Ind[id2] ][0] < float('inf'):
			return self.connectionMatrix[ self.v2Ind[id1] ][ self.v2Ind[id2] ]
		else:
			if outToStdout : print("No path exists between",id1,"-",id2)#"-> ",self.connectionMatrix[ self.v2Ind[id1] ][ self.v2Ind[id2] ])
			#print(len(self.connectionMatrix))
			return None
	
	def calculateShortestPaths( self ):
		print("Info : Calculating shortest paths for the current map")
		self.connectionMatrix =  [[(float('inf'),[]) for x in range(len(self.vertices))] for y in range(len(self.vertices))] 
		
		
		self.v2Ind.clear()
		self.v2Ind = dict()	
		
		for n in self.vertices:
			self.connectionMatrix[ len(self.v2Ind) ][ len(self.v2Ind) ] = (0, [])
			
			self.v2Ind[ n.vid ] = len(self.v2Ind)
		
		for e in self.edges:
			self.connectionMatrix[ self.v2Ind[e.edge.start.vid] ]\
					[ self.v2Ind[e.edge.end.vid] ] = (e.edge.dist,[e.edge.end.vid])
	
		for k in range(len(self.vertices)):
			for i in range(len(self.vertices)):
				for j in range(len(self.vertices)):
					n2 = self.vertices[k]
					n1 = self.vertices[i]
					n3 = self.vertices[j]
					
					ind1 = self.v2Ind[ n1.vid ]
					ind2 = self.v2Ind[ n2.vid ]
					ind3 = self.v2Ind[ n3.vid ]
					
					if ( self.connectionMatrix[ ind1 ][ ind2 ][0] + \
							self.connectionMatrix[ ind2 ][ ind3 ][0] ) < \
							self.connectionMatrix[ ind1 ][ ind3 ][0] : 
						R1 = self.connectionMatrix[ ind1 ][ ind2 ]
						R2 = self.connectionMatrix[ ind2 ][ ind3 ]
						self.connectionMatrix[ ind1 ][ ind3 ] = ( R1[0]+R2[0],R1[1]+R2[1] )
			
		self.__isChanged = True
		self.ispathschanged = False
		
	def saveMap(self,username, map_name):
	
		self.__mapname = map_name
		self.edgesToSave = []
		for rs in self.edges:
			 self.edgesToSave.append( (rs.nlen,rs.nlanes,rs.edge) )
			 
		
		self.edges.clear()
		self.edges = []
		
		dumped = pickle.dumps(self.__dict__)
		
		with sqlite3.connect("traffic_simulation.sqlite3") as db:
			cur = db.cursor()
			cur.execute("""create table if not exists maps (\
					user varchar(100),\
					map_name varchar(100),\
					map_object blob,\
					primary key(user, map_name))""")
			cur.execute("""insert into maps values (\"{}\",\"{}\", ?)"""\
					.format(username, map_name), (dumped,)) 
			db.commit()
		
		for l in self.edgesToSave:
			 self.edges.append( RSegment( l[0],l[1],l[2] ) )
		
				
	def showMaps(self, user):
		try:
			with sqlite3.connect("traffic_simulation.sqlite3") as db:
				cur = db.cursor()
				cur.execute("""select * from maps where user=\"{}\" """.format(user))
		
				for row in cur.fetchall():
					print(row)

		except Exception as e:
			print("SQL error",e)

		
	def deleteMap(self,username, map_name):
		try:
			with sqlite3.connect("traffic_simulation.sqlite3") as db:
				cur = db.cursor()
				cur.execute("delete FROM maps WHERE user=? and map_name=?", (username,map_name))
				db.commit()
				
		except Exception as e:
			print("SQL error",e)
		
	def loadMap(self,username, map_name):
		with sqlite3.connect("traffic_simulation.sqlite3") as db:
			cur = db.cursor()
			cur.execute("""select map_object\
						   from maps\
						   where map_name=\"{}\" AND\
								 user=\"{}\"\
						   limit 1""".format(map_name, username))
			
			res = cur.fetchone() 
			s = res[0]
	#		print(s) #Test
			
			pobj = pickle.loads( s )
			self.__dict__.update( pobj )
			
			
			for l in self.edgesToSave:
				 self.edges.append( RSegment( l[0],l[1],l[2] ) )
			self.edgesToSave.clear()
		
		self.__mapname = map_name
	
	
