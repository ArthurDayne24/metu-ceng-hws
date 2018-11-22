
import socket
import threading
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
        self.brokerThread = threading.Thread(target=self.worker_to_broker)
        self.brokerThread.start()

        self.destinationThread = threading.Thread(target=self.worker_to_destination)
        self.destinationThread.start()

        # end
        self.brokerThread.join()
        self.destinationThread.join()

        self.bSocket.close()
        self.dSocket.close()

    def worker_to_broker(self):
        self.bSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.bSocket.bind((self.rLeftInterface, PORT))

        # wait for other connections to be established
        self.allConnectionsEstablished.wait()

        data = bytearray()
       
        while True:
            # get data from b and transmit to d

            # receive from b
            get_data, _ = self.bSocket.recvfrom(PACKET_SIZE)
            
            data.extend(get_data)
            if len(data) < PACKET_SIZE:
                continue

            # send to d
            self.dSocket.sendto(data[:PACKET_SIZE], (self.dInterface, PORT))
            print("R1 Sent to Broker", data.decode("utf-8")[:PACKET_SIZE])
            data = data[PACKET_SIZE:]

    def worker_to_destination(self):
        self.dSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.dSocket.bind((self.rRightInterface, PORT))

        # wait for other connections to be established
        self.allConnectionsEstablished.wait()
        
        while True:
            # get (n)ack from d and transmit to b

            # receive from d
            data, _ = self.dSocket.recvfrom(ACK_SIZE)
            # TODO debug
            #print("Received ACK from destination")

            # send to b
            self.bSocket.sendto(data, (self.bInterface, PORT))

            #print("ACK sent to broker")
