
import socket
from commons import *

def get_random_file_stream():
    return bytearray(random.randint(MAX_FILE_SIZE))

class SourceNode:
    def __init__(self):
        self.bSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.bSocket.connect((INTERFACE_B, PORT_B))
        
        while True:
            # send file to broker / get (n)ack

            # read file
            file_stream = get_random_file_stream()
            
            # send to broker
            self.bSocket.send(file_stream)
            # get (n)ack
            data = bSocket.recv(HEADER_SIZE)

        # end
        self.bSocket.close()

if __name__ == '__main__':
    SourceNode()

