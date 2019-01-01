import socket
import threading
import select

from commons import *


class BrokerNode:

    def __init__(self):

        # base and next sequence numbers to be used in Go-Back-N
        self.base = 0
        self.nextseqnum = 0

        # receiver threads to receive ACK packets from destination node via two interfaces
        self.receiver_thread_1 = threading.Thread(target=self.worker_receiver, args=(0,))
        self.receiver_thread_2 = threading.Thread(target=self.worker_receiver, args=(1,))

        # > Producer / Consumer Pattern elements

        self.packets = []

        # sender threads to send packets to destination node via two interfaces
        # these threads pick packets from the queue which tcp receiver thread fills in the mean time
        self.sender_thread_1 = threading.Thread(target=self.worker_sender, args=(0,))
        self.sender_thread_2 = threading.Thread(target=self.worker_sender, args=(1,))

        # receiver thread that receives packets from source node and puts to queue
        self.tcp_receiver_thread = threading.Thread(target=self.worker_tcp_receiver)

        # Producer / Consumer Pattern elements <

        # We register last timeout handler thread to "join" at the end of file transmission
        self.timeout_handler_thread = None

        self.sSocket = self.r1Socket = self.r2Socket = None
        self.sConnectionSocket = None

        # Interface 1 (for s)
        self.sSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sSocket.bind((INTERFACE_1, PORT_1))
        self.sSocket.listen(1)  # Behaves like a server

        self.sConnectionSocket, _ = self.sSocket.accept()  # Accept and connect to a client

        # > sockets to be used for connections with routers

        self.r1Socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.r1Socket.bind((INTERFACE_2, PORT_2))

        self.r2Socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.r2Socket.bind((INTERFACE_6, PORT_6))

        # sockets to be used for connections with routers <

        self.sockets = [self.r1Socket, self.r2Socket]
        self.sendingInt = [INTERFACE_5, INTERFACE_9]
        self.sendingPort = [PORT_5, PORT_9]
        self.receiver_threads = [self.receiver_thread_1, self.receiver_thread_2]

        # protect next sequence number variable from race conditions
        self.l_nextseqnum = threading.Lock()

        # CV for producer / consumer pattern
        self.cv_packetReady = threading.Condition()
        # Last received packet's index
        self.nReceivedPacketIndex = -1

        # is self.base updated ?
        # to prevent idle while loops at sender threads
        self.cv_baseUpdated = threading.Condition()

        # timeout worker guard to satisfy "only one timeout handler works at a moment"
        self.l_is_timeout_already = threading.Lock()

    # Handles data transfer from s to d
    def run(self):

        self.tcp_receiver_thread.start()

        self.sender_thread_1.start()
        self.sender_thread_2.start()

        self.tcp_receiver_thread.join()

        self.sender_thread_1.join()
        self.sender_thread_2.join()

        self.timeout_handler_thread.join()

        self.sSocket.close()

        self.r1Socket.close()
        self.r2Socket.close()

    def worker_tcp_receiver(self):

        nextseqnum = 0

        # store and forward implementation variables
        receive_buffer = bytearray()
        received_size = 0

        while nextseqnum < NUMBER_OF_PACKETS:

            # > store and forward implementation

            while received_size < PACKET_SIZE:
                data = self.sConnectionSocket.recv(PACKET_SIZE)

                receive_buffer.extend(data)
                received_size += len(data)

            packet = receive_buffer[:PACKET_SIZE]
            receive_buffer = receive_buffer[PACKET_SIZE:]
            received_size -= PACKET_SIZE

            # < store and forward implementation

            nextseqnum += 1

            self.packets.append(packet)

            # notify sender threads as new packets are available to be sent
            with self.cv_packetReady:
                self.nReceivedPacketIndex += 1
                self.cv_packetReady.notifyAll()

    # Worker thread to send packets to destination node
    def worker_sender(self, router_id):

        # if first iteration, then invoke related receiver thread
        first_visit = True

        # functionalize sender worker to use via both sender threads with the help of router_id
        router_port = self.sendingPort[router_id]
        router_interface = self.sendingInt[router_id]
        router_socket = self.sockets[router_id]
        receiver_thread = self.receiver_threads[router_id]

        while self.base != NUMBER_OF_PACKETS:

            # > protect next sequence number

            self.l_nextseqnum.acquire()

            # prevent needless while loops checking that if there exists new packets within the window
            with self.cv_baseUpdated:
                while self.nextseqnum >= self.base + RDT_WINDOW_SIZE:
                    self.cv_baseUpdated.wait()

            # prevent needles while loops checking that if a packet is made available by source receiver (tcp receiver) thread
            with self.cv_packetReady:
                while self.nReceivedPacketIndex < self.nextseqnum:
                    self.cv_packetReady.wait()

                packet = self.packets[self.nextseqnum]

            self.nextseqnum += 1

            self.l_nextseqnum.release()

            # protect next sequence number <

            # send packet to destination
            router_socket.sendto(packet, (router_interface, router_port))

            # if first iteration, then invoke related receiver thread
            if first_visit:
                first_visit = False
                receiver_thread.start()

        receiver_thread.join()

    # Worker thread receiving ACK packets from destination node
    def worker_receiver(self, router_id):

        # functionalize receiver worker to use via both receiver threads with the help of router_id
        r_socket = self.sockets[router_id]

        # store and forward implementation variables
        receive_buffer = bytearray()
        received_size = 0

        while self.base != NUMBER_OF_PACKETS:

            # > store and forward implementation

            while received_size < PACKET_SIZE and self.base != NUMBER_OF_PACKETS:

                # wait at most for TIMEOUT seconds for an ACK packet
                r_socket.setblocking(False)

                ready = select.select([r_socket], [], [], TIMEOUT)

                # if timeout not occurred
                if ready[0]:
                    data, _ = r_socket.recvfrom(PACKET_SIZE)  # type: (bytes, object)

                    receive_buffer.extend(data)
                    received_size += len(data)

                # if timeout occurred
                else:
                    receive_buffer = bytearray()
                    received_size = 0

                    # satisfy that only one timeout thread is used at a moment
                    if not self.l_is_timeout_already.locked():
                        self.l_is_timeout_already.acquire()
                        self.timeout_handler_thread = threading.Thread(target=self.worker_timeout_handler,
                                                                       args=(router_id, self.nextseqnum,))
                        self.timeout_handler_thread.start()

            # store and forward implementation <

            if self.base == NUMBER_OF_PACKETS:
                return

            packet = receive_buffer[:PACKET_SIZE]
            receive_buffer = receive_buffer[PACKET_SIZE:]
            received_size -= PACKET_SIZE

            # if packet is corrupted, then ignore received packet
            if is_corrupted(packet):
                continue

            new_sequence_number = get_sequence_number(packet) + 1

            with self.cv_baseUpdated:
                # if ACK is received for an already ACK'ed packet, ignore it
                if new_sequence_number < self.base:
                    continue

                # update base variable and notify sender threads
                self.base = new_sequence_number
                self.cv_baseUpdated.notifyAll()

    def worker_timeout_handler(self, router_id, nextseqnum):

        # resend all packets within the window
        for packet_number in range(self.base, nextseqnum):

            # functionalize timeout handler worker to be invoked via both receiver threads with the help of router_id
            router_socket = self.sockets[router_id]
            router_interface = self.sendingInt[router_id]
            router_port = self.sendingPort[router_id]

            router_socket.sendto(self.packets[packet_number], (router_interface, router_port))

        # release lock to tell receiver threads that timeout thread is available
        self.l_is_timeout_already.release()


if __name__ == '__main__':
    BrokerNode().run()
