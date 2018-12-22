import socket
import threading
import select
from threading import Lock

from commons import *


class BrokerNode:
    l_criticalRegion = ...  # type: Lock

    def __init__(self):

        self.receiver_thread_1 = threading.Thread(target=self.worker_receiver, args=(0,))
        self.receiver_thread_2 = threading.Thread(target=self.worker_receiver, args=(1,))

        self.packets = []

        self.sSocket = self.r1Socket = self.r2Socket = None
        self.sConnectionSocket = None

        # Interface 1 (for s)
        self.sSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sSocket.bind((INTERFACE_1, PORT_1))
        self.sSocket.listen(1)  # Behaves like a server

        self.sConnectionSocket, _ = self.sSocket.accept()  # Accept and connect to a client

        self.r1Socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.r1Socket.bind((INTERFACE_2, PORT_2))

        self.r2Socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.r2Socket.bind((INTERFACE_6, PORT_6))

        self.base = 0
        self.nextseqnum = 0

        self.sockets = [self.r1Socket, self.r2Socket]
        self.sendingInt = [INTERFACE_3, INTERFACE_7]
        self.sendingPort = [PORT_3, PORT_7]
        self.local_sendingInt = [INTERFACE_5, INTERFACE_9]
        self.local_sendingPort = [PORT_5, PORT_9]

        self.l_criticalRegion = threading.Lock()

    # Handles data transfer from s to d
    def run(self):

        self.worker_sender()

        self.sSocket.close()

        self.r1Socket.close()
        self.r2Socket.close()

    def decide_path(self, seed):
        if ON_LOCAL:
            if seed % 2 == 0:
                router_interface = INTERFACE_5
                router_port = PORT_5
                router_socket = self.r1Socket
            else:
                # send to router 2
                router_interface = INTERFACE_9
                router_port = PORT_9
                router_socket = self.r2Socket
        else:
            if seed % 2 == 0:
                # send to router 1
                router_interface = INTERFACE_3
                router_port = PORT_3
                router_socket = self.r1Socket
            else:
                # send to router 2
                router_interface = INTERFACE_7
                router_port = PORT_7
                router_socket = self.r2Socket

        return router_socket, router_interface, router_port

    def worker_sender(self):

        self.base = 0
        self.nextseqnum = 0

        receive_buffer = bytearray()
        received_size = 0

        while self.nextseqnum < NUMBER_OF_PACKETS:

            if self.nextseqnum < self.base + RDT_WINDOW_SIZE:

                # prepare packet
                while received_size < PAYLOAD_SIZE:
                    data = self.sConnectionSocket.recv(PAYLOAD_SIZE)

                    receive_buffer.extend(data)
                    received_size += len(data)

                payload = receive_buffer[:PAYLOAD_SIZE]
                receive_buffer = receive_buffer[PAYLOAD_SIZE:]
                received_size -= PAYLOAD_SIZE

                sequence_number = bytearray(str(self.nextseqnum).zfill(SEQUENCE_NUM_SIZE), ENCODING)

                intermediate = payload + sequence_number

                packet = intermediate + checksum(intermediate)
                self.packets.append(packet)

                router_socket, router_interface, router_port = self.decide_path(self.nextseqnum)

                router_socket.sendto(packet, (router_interface, router_port))

                debug("Packet sent with seq: " + str(self.nextseqnum))

                # if first time, start receive worker
                if self.nextseqnum == 0:
                    self.receiver_thread_1.start()
                    self.receiver_thread_2.start()

                self.nextseqnum += 1

        self.receiver_thread_1.join()
        self.receiver_thread_2.join()

    "Receives ACK from router 1 and accomplishes resend operation"
    def worker_receiver(self, router_id):

        rSocket = self.sockets[router_id]

        receive_buffer = bytearray()
        received_size = 0

        while self.nextseqnum < NUMBER_OF_PACKETS:

            # prepare packet 
            while received_size < PACKET_SIZE:

                rSocket.setblocking(False)

                ready = select.select([rSocket], [], [], TIMEOUT)

                # if timeout not occurred
                if ready[0]:
                    data, _ = rSocket.recvfrom(PACKET_SIZE)  # type: (bytes, object)

                    receive_buffer.extend(data)
                    received_size += len(data)

                # timeout case
                else:
                    receive_buffer = bytearray()
                    received_size = 0

                    timeout_handler_thread = threading.Thread(target=self.worker_timeout_handler)

                    timeout_handler_thread.start()

            packet = receive_buffer[:PACKET_SIZE]
            receive_buffer = receive_buffer[PACKET_SIZE:]
            received_size -= PACKET_SIZE

            debug("ACK received with seq " + str(get_sequence_number(packet)))

            # if packet is corrupted

            if is_corrupted(packet):
                continue

            new_sequence_number = get_sequence_number(packet) + 1

            with self.l_criticalRegion:

                if new_sequence_number < self.base:
                    continue

                # if not corrupted
                self.base = new_sequence_number
                debug("BASE IS UPDATED: " + str(self.base))

    # timeout base is not the current base !
    def worker_timeout_handler(self):

        for packet_number in range(self.base, self.nextseqnum):
            debug("Resending " + str(packet_number))

            router_socket, router_interface, router_port = self.decide_path(packet_number)

            router_socket.sendto(self.packets[packet_number], (router_interface, router_port))


if __name__ == '__main__':
    BrokerNode().run()
