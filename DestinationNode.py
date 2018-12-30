import filecmp
import socket
import threading
import os
import time

from commons import *


class DestinationNode:

    def __init__(self):

        if os.path.exists('output.txt'):
            os.remove('output.txt')

        self.l_output_buffer = threading.Lock()
        self.written_to_buffer = False
        self.output_buffer = [None] * NUMBER_OF_PACKETS

        self.expected_sequence_num = 0
        self.l_expectedSeqNum = threading.Lock()

        self.r1Socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.r1Socket.bind((INTERFACE_5, PORT_5))

        self.r2Socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.r2Socket.bind((INTERFACE_9, PORT_9))

        self.r1Thread = threading.Thread(target=self.worker_r, args=(0,))
        self.r2Thread = threading.Thread(target=self.worker_r, args=(1,))

        self.current_ack = None

        self.sockets = [self.r1Socket, self.r2Socket]
        self.sendingInt = [INTERFACE_2, INTERFACE_6]
        self.sendingPort = [PORT_2, PORT_6]

    @staticmethod
    def prepare_ack_for_seq(sequence_number):
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
        r_socket = self.sockets[router_id]
        sending_int = self.sendingInt[router_id]
        sending_port = self.sendingPort[router_id]

        receive_buffer = bytearray()
        received_size = 0

        while self.expected_sequence_num != NUMBER_OF_PACKETS:

            # prepare packet
            while received_size < PACKET_SIZE and self.expected_sequence_num != NUMBER_OF_PACKETS:

                data, _ = r_socket.recvfrom(PACKET_SIZE)

                receive_buffer.extend(data)
                received_size += len(data)

            received_packet = receive_buffer[:PACKET_SIZE]
            receive_buffer = receive_buffer[PACKET_SIZE:]
            received_size -= PACKET_SIZE

            resent_current_ack = is_corrupted(received_packet)

            if not resent_current_ack:
                with self.l_expectedSeqNum:
                    resent_current_ack = not self.expected_sequence_num == get_sequence_number(received_packet)

            if resent_current_ack:
                if self.current_ack is not None:
                    r_socket.sendto(self.current_ack, (sending_int, sending_port))
                continue

            with self.l_expectedSeqNum:
                # Assigning to local variable to avoid unnecessary lock/release
                current_expected_sequence_num = self.expected_sequence_num
                self.expected_sequence_num += 1

            # save data to buffer
            self.output_buffer[current_expected_sequence_num] = received_packet

            # prepare ACK packet
            self.current_ack = self.prepare_ack_for_seq(current_expected_sequence_num)

            r_socket.sendto(self.current_ack, (sending_int, sending_port))

        with self.l_output_buffer:
            if self.written_to_buffer:
                return
            print("Ends at " + str(time.time()))
            with open('output.txt', 'wb+') as fd:
                fd.write(b''.join(map(lambda p: p[:PAYLOAD_SIZE], self.output_buffer)))
            self.written_to_buffer = True
            print("File transmission is successful." if filecmp.cmp('input.txt', 'output.txt') else 'Failed!')

if __name__ == '__main__':
    DestinationNode().run()
