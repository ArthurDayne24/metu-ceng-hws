
import socket
import threading
from commons import *

i = 0

def get_random_chunk():
    global i
    i += 1
    return bytearray("Hakan xd"+str(i), 'utf-8')
#    return bytearray(random.randint(MAX_FILE_SIZE))

class SourceNode:
    def __init__(self):
        self.bSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.bSocket.connect((INTERFACE_1, PORT))

        senderThread = threading.Thread(target=self.worker_sender)
        senderThread.start()

        receiverThread = threading.Thread(target=self.worker_receiver)
        receiverThread.start()

        senderThread.join()
        receiverThread.join()

        # end
        self.bSocket.close()

    def worker_sender(self):
        while True:
            # read file
            chunk = get_random_chunk()
        
            # send to broker
            self.bSocket.send(chunk)

    def worker_receiver(self):
        while True:
            # get (n)ack
            data = self.bSocket.recv(HEADER_SIZE)
            print("Receive", data.decode('utf-8'))

if __name__ == '__main__':
    SourceNode()

