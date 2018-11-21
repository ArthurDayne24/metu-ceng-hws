
import socket
from commons import *

def get_random_chunk():
    return bytearray("Hakan xd")
#    return bytearray(random.randint(MAX_FILE_SIZE))

class SourceNode:
    def __init__(self):
        self.bSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.bSocket.connect((INTERFACE_1, PORT))
        
        while True:
            # send file to broker / get (n)ack

            # read file
            chunk = get_random_chunk()
            
            # send to broker
            self.bSocket.send(chunk)
            # get (n)ack
            data = bSocket.recv(HEADER_SIZE)

        # end
        self.bSocket.close()

if __name__ == '__main__':
    SourceNode()

