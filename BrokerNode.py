import socket
import threading
import select
from commons import *


class BrokerNode:
    def __init__(self):

        self.receiver_thread = threading.Thread(target=self.worker_receiver_1)
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

        self.worker_sender()

        for thread in self.timeout_handler_threads:
            thread.join()

        self.sSocket.close()
        self.r1Socket.close()

    def worker_sender(self):

        self.base = 0
        self.nextseqnum = 0

        receive_buffer = bytearray()
        received_size = 0

        while self.nextseqnum < NUMBER_OF_PACKETS:

            if self.nextseqnum < self.base + RDT_WINDOW_SIZE:

                debug("nextseqnum: " + str(self.nextseqnum))
                debug("base: " + str(self.base))

                # prepare packet
                while received_size < PAYLOAD_SIZE:
                    data = self.sConnectionSocket.recv(PAYLOAD_SIZE)

                    receive_buffer.extend(data)
                    received_size += len(data)

                payload = receive_buffer[:PAYLOAD_SIZE]
                receive_buffer = receive_buffer[PAYLOAD_SIZE:]
                received_size -= PAYLOAD_SIZE

                debug("Generated packet")

                sequence_number = bytearray(str(self.nextseqnum).zfill(SEQUENCE_NUM_SIZE), ENCODING)

                intermediate = payload + sequence_number

                packet = intermediate + checksum(intermediate)
                self.packets.append(packet)

                # send to router 1
                if ON_LOCAL:
                    self.r1Socket.sendto(packet, (INTERFACE_5, PORT_5))
                else:
                    self.r1Socket.sendto(packet, (INTERFACE_3, PORT_3))

                debug("Packet sent")

                # TODO handle two routers

                # if first time, start receive worker
                if self.nextseqnum == 0:
                    self.receiver_thread.start()

                self.nextseqnum += 1

        self.receiver_thread.join()

    "Receives ACK from router 1 and accomplishes resend operation"
    def worker_receiver_1(self):

        receive_buffer = bytearray()
        received_size = 0

        while self.nextseqnum < NUMBER_OF_PACKETS:

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
                    receive_buffer = bytearray()
                    received_size = 0

                    debug("base [before send]" + str(self.base))

                    timeout_handler_thread = threading.Thread(target=self.worker_timeout_handler, args=(self.base,))

                    debug("base [after send]" + str(self.base))

                    self.timeout_handler_threads.append(timeout_handler_thread)
                    timeout_handler_thread.start()

            packet = receive_buffer[:PACKET_SIZE]
            receive_buffer = receive_buffer[PACKET_SIZE:]
            received_size -= PACKET_SIZE

            debug("ACK received with seq " + str(get_sequence_number(packet)))

            # if packet is corrupted

            if is_corrupted(packet):
                continue

            new_sequence_number = get_sequence_number(packet) + 1

            if new_sequence_number < self.base:
                continue

            # if not corrupted
            self.base = new_sequence_number
            debug("BASE IS UPDATED: " + str(self.base))

            if self.base == self.nextseqnum:
                continue

    # timeout base is not the current base !
    def worker_timeout_handler(self, timeout_base):
        debug("Handling timeout")

        for packet_number in range(timeout_base, self.nextseqnum):
            debug("Resending " + str(packet_number))
            if ON_LOCAL:
                self.r1Socket.sendto(self.packets[packet_number], (INTERFACE_5, PORT_5))
            else:
                self.r1Socket.sendto(self.packets[packet_number], (INTERFACE_3, PORT_3))

        debug("Exit timeout")

if __name__ == '__main__':
    BrokerNode().run()
