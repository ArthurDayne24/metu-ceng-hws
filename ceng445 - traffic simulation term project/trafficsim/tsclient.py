import json
from threading import Thread, Lock, Condition
from time import sleep
from socket import *

class Client(Thread):
	def __init__(self,ip,port):
		self.port = port
		self.ip = ip
		self.conn = None
		
		self.stateMutex = Lock()
		self.stateCond = Condition( self.stateMutex )
		self.__currentState = [[] for i in range(6)]
		
		self.connMutex = Lock()
		self.connMutex.acquire()
		
		self.__terminate = False
		
		self.willBeChanged = True
		
		super().__init__()
	
	def startConnection(self):
		self.conn = socket(AF_INET, SOCK_STREAM)
		self.conn.connect((self.ip, self.port))
		self.connMutex.release()
	
	def endConnection(self):
		self.conn.close()
	
	def sendUsingProtocol(self,dataList):
		#print("s1", dataList)
		with self.connMutex:
			dumpedByteArr = json.dumps(dataList).encode()
			self.conn.send(('{:10d}'.format(len(dumpedByteArr))).encode())
			self.conn.send( dumpedByteArr )
			self.willBeChanged = True
	
	def parseReq(self, tbParsed):
		req = []
		req.append( 1 if "CarStat" in tbParsed else 0 )
		req.append( 1 if "CarEnterExit" in tbParsed else 0 )
		req.append( 1 if "CarStarFinish" in tbParsed else 0 )
		req.append( 1 if "EdgeStat" in tbParsed else 0 )
		req.append( 1 if "SimStat" in tbParsed else 0 )
		req.append( 1 if "SimReport" in tbParsed else 0 )
		return req
	
	def getDesc(self, x):
		if x == 0:
			return "CarStat"
		if x == 1:
			return "CarEnterExit"
		if x == 2:
			return "CarStarFinish"
		if x == 3:
			return "EdgeStat"
		if x == 4:
			return "SimStat"
		if x == 5:
			return "SimReport"
			
	def listenForInput(self):
		while not self.__terminate:
			raw_inp = input()
			dumpedByteArr = json.dumps(raw_inp).encode()
			self.conn.send(('{:10d}'.format(len(dumpedByteArr))).encode())
			self.conn.send( dumpedByteArr )
	
	def getCurrentState(self):
		#print("g1")
		with self.stateCond:
		#	print("g2")
			while self.willBeChanged and not self.__terminate:
		#		print("g3")
				self.stateCond.wait(1)
				self.willBeChanged = False
			return self.__currentState
		#print("g4")
	
	def run(self):
		self.startConnection()
		#self.sendUsingProtocol( self.parseReq( ["CarStat","EdgeStat","SimStat","CarEnterExit","CarStarFinish"]))##list(input()) ) )
		#t1 = Thread(target = self.listenForInput)
		#t1.start()
		
		tcount = 0
		while not self.__terminate:
			responseLen = self.conn.recv(10)
			print( "From ts(p2)server to cl",self.port,", len = ", int(responseLen) )
			response = self.conn.recv( int(responseLen) )
			#print(" --------------------- TICK {} --------------------- ".format( tcount ))
			tcount += 1
			encodedData = json.loads(response.decode())
			#print("d1")
			with self.stateCond:
				#print("d2")
				for i,row in enumerate(encodedData):
					self.__currentState[i] = row
					#print( self.getDesc(i),":", row)
				self.willBeChanged = False
				self.stateCond.notifyAll()
				#print("d3")
					
			if encodedData[5] != []:
				break
		
		self.sendUsingProtocol( "['end_simulation']" )
		self.endConnection()
		



if __name__ == '__main__':
	c = Client('',int(input("On port :")))#20445)
	c.start()
	
	c.join()
	
