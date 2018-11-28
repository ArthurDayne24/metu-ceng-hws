
import socket
import random
import threading
import datetime
from commons import *

def get_random_message():
    return bytearray(str(random.randint(1000, 9999)) * (PACKET_SIZE // 4), 'utf-8')

class SourceNode:
    def __init__(self):
        self.bSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.bSocket.connect((INTERFACE_1, PORT))

        while True:
            # read file
            message = get_random_message()
            print("Sent", message)

            # send to broker
            self.bSocket.send(message)
            start = datetime.datetime.now()
            # get (n)ack
            data = self.bSocket.recv(ACK_SIZE)
            end = datetime.datetime.now()

            delta_time = end - start

            passed_time_miliseconds = (delta_time.seconds + delta_time.microseconds * 1e-6) * 1e3
            print("Received. Elapsed: {:.6f} miliseconds".format(passed_time_miliseconds))

        # end
        self.bSocket.close()

if __name__ == '__main__':
    SourceNode()

