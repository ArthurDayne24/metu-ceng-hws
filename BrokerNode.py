import socket
import threading
import select
from commons import *


class BrokerNode:
    def __init__(self):

        self.timeout_handler_threads = []
        self.packets = []

        self.sSocket = self.r1Socket = self.r2Socket = None
        self.sConnectionSocket = None

        # Interface 1 (for s)
        self.sSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sSocket.bind((INTERFACE_1, PORT_1))
        self.sSocket.listen(1)  # Behaves like a server

        self.sConnectionSocket, _ = self.sSocket.accept()  # Accept and connect to a client

        # Interface 2 (for r1)
        self.r1Socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.r1Socket.bind((INTERFACE_2, PORT_2))

        self.base = 0
        self.nextseqnum = 0

    # Handles data transfer from s to d
    def run(self):
        self.sSocket.close()
        self.r1Socket.close()

    def worker_sender(self):

        self.base = 0
        self.nextseqnum = 0

        receive_buffer = bytearray()
        received_size = 0

        while True:

            if self.nextseqnum < self.base + BROKER_WINDOW_SIZE:

                # prepare packet
                while received_size < PAYLOAD_SIZE:
                    data = self.sConnectionSocket.recv(PAYLOAD_SIZE)  # receive from s

                    receive_buffer.extend(data)
                    received_size += len(data)

                payload = receive_buffer[:PAYLOAD_SIZE]
                receive_buffer = receive_buffer[PAYLOAD_SIZE:]
                received_size -= PAYLOAD_SIZE

                sequence_number = bytearray(str(self.nextseqnum).zfill(SEQUENCE_NUM_SIZE))

                intermediate = payload + sequence_number

                packet = intermediate + checksum(intermediate)
                self.packets.append(packet)

                # send to router 1
                self.r1Socket.sendto(packet, (INTERFACE_3, PORT_3))

                # TODO handle two routers

                if self.base == self.nextseqnum:
                    pass

                    # TODO start timer

                self.nextseqnum += 1

            else:
                pass

    "Receives ACK from router 1 and accomplishes resend operation"
    def worker_receiver_1(self):

        receive_buffer = bytearray()
        received_size = 0

        while True:

            # prepare packet 
            while received_size < PACKET_SIZE:

                self.r1Socket.setblocking(False)

                ready = select.select([self.r1Socket], [], [], TIMEOUT)

                # if timeout not occurred
                if ready[0]:
                    data, _ = self.r1Socket.recvfrom(PACKET_SIZE)  # type: (bytes, object)

                    receive_buffer.extend(data)
                    received_size += len(data)

                # timeout case
                else:
                    receive_buffer = []

                    self.timeout_handler_thread = threading.Thread(target=self.worker_timeout_handler, args=(self.base,))
                    self.timeout_handler_threads.append(self.timeout_handler_thread)
                    self.timeout_handler_thread.start()

            packet = receive_buffer[:PACKET_SIZE]
            receive_buffer = receive_buffer[PACKET_SIZE:]
            received_size -= PACKET_SIZE

            intermediate = packet[:PAYLOAD_SIZE + SEQUENCE_NUM_SIZE]
            packet_checksum = packet[PAYLOAD_SIZE + SEQUENCE_NUM_SIZE:]
            sequence_number = intermediate[PAYLOAD_SIZE:]

            # if packet is corrupted

            if checksum(intermediate) != packet_checksum:
                continue

            # if not corrupted

            self.base = int(sequence_number) + 1

            if self.base == self.nextseqnum:
                return  # no more packets

    # timeout base is not the current base !
    def worker_timeout_handler(self, timeout_base):
        for packet_number in range(timeout_base, self.nextseqnum):
            self.r1Socket.sendto(self.packets[packet_number], (INTERFACE_3, PORT_3))

if __name__ == '__main__':
    BrokerNode().run()
