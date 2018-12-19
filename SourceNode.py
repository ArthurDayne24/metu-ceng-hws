import socket
import random
import threading
import datetime
from commons import *

# Construct a random message string with a repeated pattern.
def get_random_message():
    return bytearray(str(random.randint(1000, 9999)) * (PAYLOAD_SIZE // 4), 'utf-8')

class SourceNode:
    def __init__(self):

        #Initialize sockets
        self.bSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.bSocket.connect((INTERFACE_1, PORT_1)) #Connect to the TCP part of broker node
        
    def run():
        sent_size = 0

        while sent_size < TOTAL_BYTES:
            # construct a message
            payload = get_random_message()

            self.bSocket.send(payload) #send message to broker over the open TCP connection

            sent_size += PAYLOAD_SIZE

        # end
        self.bSocket.close()

if __name__ == '__main__':
    SourceNode().run()

