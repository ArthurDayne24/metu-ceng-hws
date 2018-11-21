
import socket
import threading 
from commons import *

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
        # Interface 5 (for r1)
        self.r1Socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.r1Socket.bind((INTERFACE_5, PORT))

        while True:
            # receive from r1
            data, _ = r1Socket.recvfrom(MAX_FILE_SIZE)
            # convert to bytearray
            data = get_binary_from_string(data)

            # send to s
            self.r1Socket.sendto(ACK_MESSAGE, (INTERFACE_4, PORT))

    def worker_r2():
        # Interface 9 (for r2)
        self.r2Socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.r2Socket.bind((INTERFACE_9, PORT))

        while True:
            # receive from r2
            data, _ = r2Socket.recvfrom(MAX_FILE_SIZE)
            # convert to bytearray
            data = get_binary_from_string(data)

            # send to s
            self.r2Socket.sendto(ACK_MESSAGE, (INTERFACE_8, PORT))

if __name__ == '__main__':
    DestinationNode()

