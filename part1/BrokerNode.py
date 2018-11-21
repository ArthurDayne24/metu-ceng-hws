
import socket
import Threading
from commons import *

class BrokerNode:
    def __init__(self):
        # sConection, r1Socket and r2Socket must wait for each other to be init
        self.allConnectionsEstablished = threading.Barrier(3)

        self.sSocket = self.r1Socket = self.r2Socket = None
        self.sConnectionSocket = None

        # init threads
        self.sForwardThread = threading.Thread(target=self.worker_forward_from_s, args=(,))
        self.sForwardThread.start()

        self.r1BackwardThread = threading.Thread(target=self.worker_backward_from_r1, args=(,))
        self.r1BackwardThread.start()

        self.r2BackwardThread = threading.Thread(target=self.worker_backward_from_r2, args=(,))
        self.r2BackwardThread.start()

        # end
        self.sForwardThread.join()
        self.r1BackwardThread.join()
        self.r2BackwardThread.join()

        self.sSocket.close()
        self.r1Socket.close()
        self.r2Socket.close()

    # Handles data transfer from s to d
    def worker_forward_from_s():
        # Interface 1 (for s)
        self.sSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sSocket.bind((INTERFACE_1, PORT))
        self.sSocket.listen(1)

        self.sConnectionSocket, _ = self.sSocket.accept()
        
        binaryCtr = 0

        # wait for other connections to be established
        self.allConnectionsEstablished.wait)

        while True:
            # transmit file from s to r1/r2

            # receive from s
            data = self.sConnectionSocket.recv(MAX_FILE_SIZE)

            # TODO convert to UDP data

            binaryCtr = (binaryCtr + 1) % 2

            # r1's turn
            if binaryCtr == 0:
                self.r1Socket.sendto(data, (INTERFACE_2, PORT))
            # r2's turn
            else: 
                self.r2Socket.sendto(data, (INTERFACE_6, PORT))
    
    def worker_backward_from_r1():
        # Interface 2 (for r1)
        self.r1Socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.r1Socket.bind((INTERFACE_2, PORT))

        # wait for other connections to be established
        self.allConnectionsEstablished.wait()
        
        while True:
            # get (n)ack from r1 and transmit to s

            # receive from r1
            data, _ = r1Socket.recvfrom(HEADER_SIZE)

            # TODO convert to TCP data
            
            # send to s
            self.sConnectionSocket.send(data)

    def worker_backward_from_r2():
        # Interface 6 (for r2)
        self.r2Socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.r2Socket.bind((INTERFACE_6, PORT))

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
    BrokerNode()

