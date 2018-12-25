import socket
import threading
import select
from threading import Lock

from commons import *


class BrokerNode:
    l_base = ...  # type: Lock

    def __init__(self):

        self.base = 0
        self.nextseqnum = 0

        self.receiver_thread_1 = threading.Thread(target=self.worker_receiver, args=(0,))
        self.receiver_thread_2 = threading.Thread(target=self.worker_receiver, args=(1,))

        self.sender_thread_1 = threading.Thread(target=self.worker_sender, args=(0,))
        self.sender_thread_2 = threading.Thread(target=self.worker_sender, args=(1,))

        self.tcp_receiver_thread = threading.Thread(target=self.worker_tcp_receiver)

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

        self.sockets = [self.r1Socket, self.r2Socket]
        self.sendingInt = [INTERFACE_5, INTERFACE_9]
        self.sendingPort = [PORT_5, PORT_9]
        self.receiver_threads = [self.receiver_thread_1, self.receiver_thread_2]

        self.l_base = threading.Lock()
        self.l_tcpReceive = threading.Lock()
        self.l_nextseqnum = threading.Lock()

        # CV for producer / consumer pattern
        self.cv_packetReady = threading.Condition()
        self.nreceivedPackets = 0

        # Timeout guard
        self.is_timeout_already = False
        self.l_is_timeout_already = threading.Lock()

    # Handles data transfer from s to d
    def run(self):

        self.tcp_receiver_thread.start()

        self.sender_thread_1.start()
        self.sender_thread_2.start()

        self.sender_thread_1.join()
        self.sender_thread_2.join()

        self.tcp_receiver_thread.join()

        self.sSocket.close()

        self.r1Socket.close()
        self.r2Socket.close()

    def worker_tcp_receiver(self):

        nextseqnum = 0

        receive_buffer = bytearray()
        received_size = 0

        while nextseqnum < NUMBER_OF_PACKETS:

            # prepare packet
            while received_size < PAYLOAD_SIZE:
                data = self.sConnectionSocket.recv(PAYLOAD_SIZE)

                receive_buffer.extend(data)
                received_size += len(data)

            payload = receive_buffer[:PAYLOAD_SIZE]
            receive_buffer = receive_buffer[PAYLOAD_SIZE:]
            received_size -= PAYLOAD_SIZE

            sequence_number = bytearray(str(nextseqnum).zfill(SEQUENCE_NUM_SIZE), ENCODING)

            nextseqnum += 1
c
            intermediate = payload + sequence_number

            packet = intermediate + checksum(intermediate)

            self.packets.append(packet)

            with self.cv_packetReady:
                self.nreceivedPackets += 1
                self.cv_packetReady.notifyAll()

    def worker_sender(self, routerId):

        first_visit = True

        router_port = self.sendingPort[routerId]
        router_interface = self.sendingInt[routerId]
        router_socket = self.sockets[routerId]
        receiver_thread = self.receiver_threads[routerId]

        while True:

            self.l_nextseqnum.locked()
            debug("locked at 124")

            if self.nextseqnum == NUMBER_OF_PACKETS:
                debug("release at 126")
                self.l_nextseqnum.release()
                break

            if self.nextseqnum < self.base + RDT_WINDOW_SIZE:

                with self.cv_packetReady:
                    while self.nreceivedPackets < self.nextseqnum:
                        self.cv_packetReady.wait()

                    packet = self.packets[self.nextseqnum]

                debug("Packet sent with seq: " + str(self.nextseqnum))

                self.nextseqnum += 1

                debug("release at 143")
                self.l_nextseqnum.release()

                router_socket.sendto(packet, (router_interface, router_port))

                # if first time, start receive worker
                if first_visit:
                    first_visit = False
                    receiver_thread.start()

            else:
                debug("released at 154")
                self.l_nextseqnum.release()

        receiver_thread.join()

    "Receives ACK from router 1 and accomplishes resend operation"
    def worker_receiver(self, routerId):

        rSocket = self.sockets[routerId]

        receive_buffer = bytearray()
        received_size = 0

        while True:

            self.l_nextseqnum.acquire()

            if self.nextseqnum == NUMBER_OF_PACKETS:
                debug("released at 171")
                self.l_nextseqnum.release()
                break

            debug("released at 176")
            self.l_nextseqnum.release()

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

                    timeout_handler_thread = threading.Thread(target=self.worker_timeout_handler, args=(routerId, self.nextseqnum,))

                    timeout_handler_thread.start()

            packet = receive_buffer[:PACKET_SIZE]
            receive_buffer = receive_buffer[PACKET_SIZE:]
            received_size -= PACKET_SIZE

            debug("ACK received with seq " + str(get_sequence_number(packet)))

            # if packet is corrupted

            if is_corrupted(packet):
                continue

            new_sequence_number = get_sequence_number(packet) + 1

            with self.l_base:

                if new_sequence_number < self.base:
                    continue

                # if not corrupted
                self.base = new_sequence_number
                debug("BASE IS UPDATED: " + str(self.base))

    # timeout base is not the current base !
    def worker_timeout_handler(self, routerId, nextseqnum):

        with self.l_is_timeout_already:
            if self.is_timeout_already:
                return
            else:
                self.is_timeout_already = True

        for packet_number in range(self.base, nextseqnum):
            debug("Resending " + str(packet_number))

            router_socket = self.sockets[routerId]
            router_interface = self.sendingInt[routerId]
            router_port = self.sendingPort[routerId]

            router_socket.sendto(self.packets[packet_number], (router_interface, router_port))

        with self.l_is_timeout_already:
            self.is_timeout_already = False


if __name__ == '__main__':
    BrokerNode().run()
