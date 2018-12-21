import socket
import threading
import random
from commons import *


class DestinationNode:

    def __init__(self):
        self.expected_sequence_num = 0
        self.r1Socket = self.r2Socket = None

        self.r1Thread = threading.Thread(target=self.worker_r1)

        self.current_ack = None

    def run(self):
        self.r1Thread.start()

        self.r1Thread.join()

        self.r1Socket.close()

    def worker_r1(self):
        # Interface 5 (for r1)
        self.r1Socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.r1Socket.bind((INTERFACE_5, PORT_5))

        receive_buffer = bytearray()
        received_size = 0

        while self.expected_sequence_num < NUMBER_OF_PACKETS:

            # prepare packet
            while received_size < PACKET_SIZE:
                data, _ = self.r1Socket.recvfrom(PACKET_SIZE)

                receive_buffer.extend(data)
                received_size += len(data)

            received_packet = receive_buffer[:PACKET_SIZE]
            receive_buffer = receive_buffer[PACKET_SIZE:]
            received_size -= PACKET_SIZE

            # TODO remove this part
            p = random.uniform(0, 1)
            if p < 0.4:
                debug("Skipped")
                debug(received_packet)
                continue


            if is_corrupted(received_packet) or not self.expected_sequence_num == get_sequence_number(received_packet):

                if ON_LOCAL:
                    self.r1Socket.sendto(self.current_ack, (INTERFACE_2, PORT_2))
                else:
                    self.r1Socket.sendto(self.current_ack, (INTERFACE_4, PORT_4))
                continue

            # prepare ACK packet
            payload = bytearray("1" * PAYLOAD_SIZE, ENCODING)
            sequence_number = bytearray(str(self.expected_sequence_num).zfill(SEQUENCE_NUM_SIZE), ENCODING)
            intermediate = payload + sequence_number
            self.current_ack = intermediate + checksum(intermediate)

            if ON_LOCAL:
                self.r1Socket.sendto(self.current_ack, (INTERFACE_2, PORT_2))
            else:
                self.r1Socket.sendto(self.current_ack, (INTERFACE_4, PORT_4))

            debug("sent packet")
            debug(self.current_ack)

            debug("Received packet")
            debug(received_packet)

            self.expected_sequence_num += 1


if __name__ == '__main__':
    DestinationNode().run()
