import socket
import threading
import select
from threading import Lock

from commons import *


class BrokerNode:

    def __init__(self):

        self.base = 0
        self.nextseqnum = 0

        self.receiver_thread_1 = threading.Thread(target=self.worker_receiver, args=(0,))
        self.receiver_thread_2 = threading.Thread(target=self.worker_receiver, args=(1,))

        self.sender_thread_1 = threading.Thread(target=self.worker_sender, args=(0,))
        self.sender_thread_2 = threading.Thread(target=self.worker_sender, args=(1,))

        self.tcp_receiver_thread = threading.Thread(target=self.worker_tcp_receiver)

        self.timeout_handler_thread = None

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

        self.l_tcpReceive = threading.Lock()
        self.l_nextseqnum = threading.Lock()

        # CV for producer / consumer pattern
        self.cv_packetReady = threading.Condition()
        # Last received packet's index
        self.nReceivedPacketIndex = -1

        # Is self.base update
        self.cv_baseUpdated = threading.Condition()

        # Timeout guard
        self.l_is_timeout_already = threading.Lock()

    # Handles data transfer from s to d
    def run(self):

        self.tcp_receiver_thread.start()

        self.sender_thread_1.start()
        self.sender_thread_2.start()

        self.tcp_receiver_thread.join()

        self.timeout_handler_thread.join()

        self.sender_thread_1.join()
        self.sender_thread_2.join()

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

            intermediate = payload + sequence_number

            packet = intermediate + checksum(intermediate)

            self.packets.append(packet)

            with self.cv_packetReady:
                self.nReceivedPacketIndex += 1
                self.cv_packetReady.notifyAll()


    def worker_sender(self, router_id):

        first_visit = True

        router_port = self.sendingPort[router_id]
        router_interface = self.sendingInt[router_id]
        router_socket = self.sockets[router_id]
        receiver_thread = self.receiver_threads[router_id]

        while True:

            self.l_nextseqnum.acquire()

            with self.cv_baseUpdated:
                while self.nextseqnum >= self.base + RDT_WINDOW_SIZE:
                    self.cv_baseUpdated.wait()

            with self.cv_packetReady:
                while self.nReceivedPacketIndex < self.nextseqnum:
                    self.cv_packetReady.wait()

                packet = self.packets[self.nextseqnum]

            self.nextseqnum += 1

            self.l_nextseqnum.release()

            router_socket.sendto(packet, (router_interface, router_port))

            # if first time, start receive worker
            if first_visit:
                first_visit = False
                receiver_thread.start()

        receiver_thread.join()

    "Receives ACK from router 1 and accomplishes resend operation"
    def worker_receiver(self, router_id):

        r_socket = self.sockets[router_id]

        receive_buffer = bytearray()
        received_size = 0

        while self.base != NUMBER_OF_PACKETS:

            # prepare packet
            while received_size < PACKET_SIZE:

                r_socket.setblocking(False)

                ready = select.select([r_socket], [], [], TIMEOUT)

                # if timeout not occurred
                if ready[0]:
                    data, _ = r_socket.recvfrom(PACKET_SIZE)  # type: (bytes, object)

                    receive_buffer.extend(data)
                    received_size += len(data)

                # timeout case
                else:
                    receive_buffer = bytearray()
                    received_size = 0

                    if not self.l_is_timeout_already.locked():
                        self.l_is_timeout_already.acquire()
                        self.timeout_handler_thread = threading.Thread(target=self.worker_timeout_handler, args=(router_id, self.nextseqnum,))
                        self.timeout_handler_thread.start()

            packet = receive_buffer[:PACKET_SIZE]
            receive_buffer = receive_buffer[PACKET_SIZE:]
            received_size -= PACKET_SIZE

            debug("Received ack for " + str(get_sequence_number(packet)))

            # if packet is corrupted
            if is_corrupted(packet):
                debug("Ack packet for seq " + str(get_sequence_number(packet)) + " is corrupted")
                continue

            new_sequence_number = get_sequence_number(packet) + 1

            with self.cv_baseUpdated:
                if new_sequence_number < self.base:
                    continue
                # if not corrupted
                self.base = new_sequence_number
                debug("Base is updated to " + str(self.base))
                self.cv_baseUpdated.notifyAll()

    # timeout base is not the current base !
    def worker_timeout_handler(self, router_id, nextseqnum):

        for packet_number in range(self.base, nextseqnum):

            router_socket = self.sockets[router_id]
            router_interface = self.sendingInt[router_id]
            router_port = self.sendingPort[router_id]

            router_socket.sendto(self.packets[packet_number], (router_interface, router_port))

            debug("Resent packet with seq " + str(packet_number))

        self.l_is_timeout_already.release()

if __name__ == '__main__':
    BrokerNode().run()
