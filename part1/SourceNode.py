import socket
import random
import threading
import datetime
from commons import *

# Construct a random message string with a repeated pattern.
def get_random_message():
    return bytearray(str(random.randint(1000, 9999)) * (PACKET_SIZE // 4), 'utf-8')

class SourceNode:
    def __init__(self):

        #Initialize sockets
        self.bSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.bSocket.connect((INTERFACE_1, PORT)) #Connect to the TCP part of broker node

        while True:
            # construct a message
            message = get_random_message()

            
            self.bSocket.send(message) #send message to broker over the open TCP connection
            start = datetime.datetime.now() #save the exact time-stamp of the moment after send operation
            data = self.bSocket.recv(ACK_SIZE) #get the acknowledgment from broker via TCP
            end = datetime.datetime.now() #save the exact time-stamp of the moment after receive operation

            #Calculate RTT
            delta_time = end - start  
            passed_time_miliseconds = (delta_time.seconds + delta_time.microseconds * 1e-6) * 1e3 

        # end
        self.bSocket.close()

if __name__ == '__main__':
    SourceNode()

