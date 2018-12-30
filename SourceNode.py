import socket
import time
from commons import *


class SourceNode:
    def __init__(self):
        # Initialize sockets
        self.bSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.bSocket.connect((INTERFACE_1, PORT_1))  # Connect to the TCP part of broker node

    def run(self):
        sent_size = 0
        nextseqnum = 0

        print("Started at " + str(time.time()))

        with open('input.txt', 'rb') as fd:

            while sent_size < TOTAL_BYTES:
                # construct a message
                payload = fd.read(PAYLOAD_SIZE)

                sequence_number = bytearray(str(nextseqnum).zfill(SEQUENCE_NUM_SIZE), ENCODING)

                intermediate = payload + sequence_number

                packet = intermediate + checksum(intermediate)

                self.bSocket.send(packet)  # send message to broker over the open TCP connection

                sent_size += PAYLOAD_SIZE
                nextseqnum += 1

        self.bSocket.close()


if __name__ == '__main__':
    SourceNode().run()
