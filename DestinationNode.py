import filecmp
import socket
import threading
import os
import time

from commons import *


class DestinationNode:

    def __init__(self):

        # remove old output.txt for convenience
        if os.path.exists('output.txt'):
            os.remove('output.txt')

        # store received packets in a protected buffer
        self.output_buffer = [None] * NUMBER_OF_PACKETS
        self.l_output_buffer = threading.Lock()

        # if all packets are written to buffer, then start dumping to file
        self.written_to_buffer = False

        # expected sequence number variable to accept / ignore received packets from broker node
        self.expected_sequence_num = 0
        # protect that variable for race conditions
        self.l_expectedSeqNum = threading.Lock()

        # > sockets to be used for connections with routers

        self.r1Socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.r1Socket.bind((INTERFACE_5, PORT_5))

        self.r2Socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.r2Socket.bind((INTERFACE_9, PORT_9))

        # sockets to be used for connections with routers <

        # sender & receiver threads to send / receive packets to / from destination node via two interfaces
        self.r1Thread = threading.Thread(target=self.worker_r, args=(0,))
        self.r2Thread = threading.Thread(target=self.worker_r, args=(1,))

        # last successful ACK received to send in the case of failure of new packets
        self.current_ack = None

        self.sockets = [self.r1Socket, self.r2Socket]
        self.sendingInt = [INTERFACE_2, INTERFACE_6]
        self.sendingPort = [PORT_2, PORT_6]

    @staticmethod
    def prepare_ack_for_seq(sequence_number):
        # ACK packet format is "111...11" (length of PAYLOAD_SIZE) + sequence number of ACK'ed packet + checksum of this ACK packet
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
        # functionalize sender / receiver worker to use via both sender / receiver threads with the help of router_id
        r_socket = self.sockets[router_id]
        sending_int = self.sendingInt[router_id]
        sending_port = self.sendingPort[router_id]

        # store and forward implementation variables
        receive_buffer = bytearray()
        received_size = 0

        while self.expected_sequence_num != NUMBER_OF_PACKETS:

            # > store and forward implementation
            while received_size < PACKET_SIZE and self.expected_sequence_num != NUMBER_OF_PACKETS:

                data, _ = r_socket.recvfrom(PACKET_SIZE)

                receive_buffer.extend(data)
                received_size += len(data)

            # store and forward implementation <

            received_packet = receive_buffer[:PACKET_SIZE]
            receive_buffer = receive_buffer[PACKET_SIZE:]
            received_size -= PACKET_SIZE

            # should we resend last successful ACK ? (corruption)
            resent_current_ack = is_corrupted(received_packet)

            if not resent_current_ack:
                with self.l_expectedSeqNum:
                    # should we resend last successful ACK ? (packet has the expected sequence number)
                    resent_current_ack = not self.expected_sequence_num == get_sequence_number(received_packet)

            if resent_current_ack:
                # if it is not the first packet that corrupted, then send last successful transmission's ACK packet
                if self.current_ack is not None:
                    r_socket.sendto(self.current_ack, (sending_int, sending_port))
                continue

            # update expected sequence number wrt received packet's sequence number
            with self.l_expectedSeqNum:
                # Assigning to local variable to avoid unnecessary lock/release
                current_expected_sequence_num = self.expected_sequence_num
                self.expected_sequence_num += 1

            # save data to buffer
            self.output_buffer[current_expected_sequence_num] = received_packet

            # prepare ACK packet
            self.current_ack = self.prepare_ack_for_seq(current_expected_sequence_num)

            # send ACK packet
            r_socket.sendto(self.current_ack, (sending_int, sending_port))

        # at the end, write buffered packets to file
        # print timestamp for calculation of file transfer time
        # print if received file is corrupted or not to be sure
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
