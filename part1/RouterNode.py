
import socket
import Threading
from commons import *

class RouterNode:
    def __init__(self, bInterface, rLeftInterface, rRightInterface, dInterface):
        self.bInterface = bInterface
        self.rLeftInterface = rLeftInterface
        self.rRightInterface = rRightInterface
        self.dInterface = dInterface

        # bSocket and dSocket must wait for each other to be init
        self.allConnectionsEstablished = threading.Barrier(2)

        self.bSocket = self.dSocket = None

        # init threads
        self.brokerThread = threading.Thread(target=self.worker_to_broker, args=(,))
        self.brokerThread.start()

        self.destinationThread = threading.Thread(target=self.worker_to_destination, args=(,))
        self.destinationThread.start()

        # end
        self.brokerThread.join()
        self.destinationThread.join()

        self.bSocket.close()
        self.dSocket.close()

    def worker_to_broker():
        self.bSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.bSocket.bind((self.rLeftInterface, PORT))

        # wait for other connections to be established
        self.allConnectionsEstablished.wait()
        
        while True:
            # get data from b and transmit to d

            # receive from b
            data, _ = bSocket.recvfrom(MAX_FILE_SIZE)

            # send to d
            self.dSocket.sendto(data, (self.bInterface, PORT))

    def worker_to_destination():
        self.dSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.dSocket.bind((self.rRightInterface, PORT))

        # wait for other connections to be established
        self.allConnectionsEstablished.wait()
        
        while True:
            # get (n)ack from d and transmit to b

            # receive from d
            data, _ = dSocket.recvfrom(HEADER_SIZE)

            # send to b
            self.bSocket.sendto(data, (self.dInterface, PORT))

