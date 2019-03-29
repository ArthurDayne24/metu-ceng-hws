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

        # print timestamp to measure file transfer time by hand at the end of the day
        print("Started at " + str(time.time()))

        with open('input.txt', 'rb') as fd:

            while sent_size < TOTAL_BYTES:

                # > construct a message

                payload = fd.read(PAYLOAD_SIZE)

                # prepare sequence number part
                sequence_number = bytearray(str(nextseqnum).zfill(SEQUENCE_NUM_SIZE), ENCODING)

                # for checksum
                intermediate = payload + sequence_number

                # concat payload, sequence number and checksum parts
                packet = intermediate + checksum(intermediate)

                # construct a message <

                # send message to broker over the open TCP connection
                self.bSocket.send(packet)

                sent_size += PAYLOAD_SIZE
                nextseqnum += 1

        self.bSocket.close()


if __name__ == '__main__':
    SourceNode().run()
