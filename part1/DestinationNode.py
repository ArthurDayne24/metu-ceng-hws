
import socket
import threading 
from commons import *

def worker_connectionsocket(connectionsocket, clientid):
    # get max size of a file
    data = connectionsocket.recv(HEADER_SIZE)
    MAX_FILE_SIZE = get_int_from_binary(data)
    # send ack
    connectionsocket.send(get_binary_from_string(ACK_MESSAGE))

    while True:
        # get each file / chunk

        data = connectionsocket.recv(MAX_FILE_SIZE)
        print("Get data from", clientid)
        time.sleep(random.uniform(0.2, 1.0))
        connectionsocket.send(get_binary_from_string(ACK_MESSAGE))
        print("Sent data to", clientid)

def main():
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.bind((BIND_IP, port))
    serversocket.listen(MAX_CONNECTIONS)
    
    nconnections = 0

    while nconnections < MAX_CONNECTIONS:
        nconnections += 1
        
        connectionsocket, address = serversocket.accept()
        print("Welcome Client nconnections", nconnections)

        thread = threading.Thread(target=worker_connectionsocket, 
                args=(connectionsocket, nconnections,))
        thread.start()

    serversocket.close()

if __name__ == '__main__':
    main()




class DestinationNode:
    def __init__(self):
        self.r1Socket = self.r2Socket = None

        # init threads
        self.r1Thread = threading.Thread(target=self.worker_r1, args=(,))
        self.r1Thread.start()

        self.r2Thread = threading.Thread(target=self.worker_r2, args=(,))
        self.r2Thread.start()

        # end
        self.r1Thread.join()
        self.r2Thread.join()

        self.r1Socket.close()
        self.r2Socket.close()

    def worker_r1():
        # Interface 2 (for r1)
        self.r1Socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.r1Socket.bind((INTERFACE_R1, PORT_R1))

        # wait for other connections to be established
        self.allConnectionsEstablished.wait()
        
        while True:
            # get (n)ack from r1 and transmit to s

            # receive from r1
            data, _ = r1Socket.recvfrom(HEADER_SIZE)
            # convert to bytearray
            data = get_binary_from_string(data)

            # TODO convert to TCP data
            
            # send to s
            self.sConnectionSocket.send(data)

    def worker_r2():
        # Interface 6 (for r2)
        self.r2Socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.r2Socket.bind((INTERFACE_R2, PORT_R2))

        # wait for other connections to be established
        self.allConnectionsEstablished.wait()
        
        while True:
            # get (n)ack from r2 and transmit to s

            # receive from r1
            data, _ = r1Socket.recvfrom(HEADER_SIZE)
            # convert to bytearray
            data = get_binary_from_string(data)

            # TODO convert to TCP data

            # send to s
            self.sConnectionSocket.send(data)

if __name__ == '__main__':
    DestinationNode()

