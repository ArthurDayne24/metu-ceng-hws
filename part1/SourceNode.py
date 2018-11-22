
import socket
import random
import threading
from commons import *

def get_random_message():
    return bytearray(str(random.randint(1000, 9999)) * (PACKET_SIZE // 4), 'utf-8')

class SourceNode:
    def __init__(self):
        self.bSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.bSocket.connect((INTERFACE_1, PORT))

        senderThread = threading.Thread(target=self.worker_sender)
        senderThread.start()

        #receiverThread = threading.Thread(target=self.worker_receiver)
        #receiverThread.start()

        senderThread.join()
        #receiverThread.join()

        # end
        self.bSocket.close()

    def worker_sender(self):
        for _ in range(10000):
            # read file
            message = get_random_message()
            print("Sent", message)
        
            # send to broker
            self.bSocket.send(message)

    def worker_receiver(self):
        while True:
            # get (n)ack
            data = self.bSocket.recv(ACK_SIZE)
#            print("Receive", data.decode('utf-8'))

if __name__ == '__main__':
    SourceNode()

