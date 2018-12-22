import socket
import threading
import random
from commons import *


class DestinationNode:

    def __init__(self):
        self.expected_sequence_num = 0

        self.r1Socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.r1Socket.bind((INTERFACE_5, PORT_5))

        self.r2Socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.r2Socket.bind((INTERFACE_9, PORT_9))

        self.r1Thread = threading.Thread(target=self.worker_r, args=(0,))
        self.r2Thread = threading.Thread(target=self.worker_r, args=(1,))

        self.current_ack = None

        self.sockets = [self.r1Socket, self.r2Socket]
        self.sendingInt = [INTERFACE_4, INTERFACE_8]
        self.sendingPort = [PORT_4, PORT_8]
        self.local_sendingInt = [INTERFACE_2, INTERFACE_6]
        self.local_sendingPort = [PORT_2, PORT_6]

        self.l_criticalRegion = threading.Lock()

    def prepare_ack_for_seq(self, sequence_number):
        payload = bytearray("1" * PAYLOAD_SIZE, ENCODING)
        sequence_number = bytearray(str(sequence_number).zfill(SEQUENCE_NUM_SIZE), ENCODING)
        intermediate = payload + sequence_number

        return intermediate + checksum(intermediate)

    def run(self):
        self.r1Thread.start()
        self.r2Thread.start()

        self.r1Thread.join()
        self.r2Thread.join()

        self.r1Socket.close()
        self.r2Socket.close()

    def worker_r(self, router_id):
        rSocket = self.sockets[router_id]
        sendingInt = self.sendingInt[router_id]
        sendingPort = self.sendingPort[router_id]
        local_sendingInt = self.local_sendingInt[router_id]
        local_sendingPort = self.local_sendingPort[router_id]

        receive_buffer = bytearray()
        received_size = 0

        # TODO care hanging thread problem for last packets

        while self.expected_sequence_num < NUMBER_OF_PACKETS:

            # prepare packet
            while received_size < PACKET_SIZE:
                data, _ = rSocket.recvfrom(PACKET_SIZE)

                receive_buffer.extend(data)
                received_size += len(data)

            received_packet = receive_buffer[:PACKET_SIZE]
            receive_buffer = receive_buffer[PACKET_SIZE:]
            received_size -= PACKET_SIZE

            # TODO remove this part
            p = random.uniform(0, 1)
            if p < 0.05:
                debug("Skipped ack packet with seq: " + str(self.expected_sequence_num))
                continue

            with self.l_criticalRegion:

                if is_corrupted(received_packet) or not self.expected_sequence_num == get_sequence_number(received_packet):

                    if self.current_ack is None:
                        continue

                    if ON_LOCAL:
                        rSocket.sendto(self.current_ack, (local_sendingInt, local_sendingPort))
                    else:
                        rSocket.sendto(self.current_ack, (sendingInt, sendingPort))

                    continue

                # prepare ACK packet
                self.current_ack = self.prepare_ack_for_seq(self.expected_sequence_num)

                if ON_LOCAL:
                    rSocket.sendto(self.current_ack, (local_sendingInt, local_sendingPort))
                else:
                    rSocket.sendto(self.current_ack, (sendingInt, sendingPort))

                debug("sent ack for seq: " + str(self.expected_sequence_num))

                self.expected_sequence_num += 1

if __name__ == '__main__':
    DestinationNode().run()
