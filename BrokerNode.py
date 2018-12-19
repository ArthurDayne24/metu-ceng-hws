import socket
import threading
from commons import *

class BrokerNode:
    def __init__(self):

        self.sSocket = self.r1Socket = self.r2Socket = None
        self.sConnectionSocket = None

        # Interface 1 (for s)
        self.sSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sSocket.bind((INTERFACE_1, PORT_1)) 
        self.sSocket.listen(1) # Behaves like a server

        self.sConnectionSocket, _ = self.sSocket.accept() #Accept and connect to a client

        # Interface 2 (for r1)
        self.r1Socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.r1Socket.bind((INTERFACE_2, PORT_2))

        self.base_changed = 0

        self.base = 0
        self.nextseqnum = 0

    # Handles data transfer from s to d
    def run(self):
        pass

        self.sSocket.close()
        self.r1Socket.close()

    def worker_r1(self):

        # get (n)ack from r1 and transmit to s

        # receive from r1
        data, _ = self.r1Socket.recvfrom(PACKET_SIZE)

        self.received.notify()        

    def worker_sender(self):

        # TODO handle buffering logic
        window_buffer = []

        self.base = 0
        self.nextseqnum = 0

        receive_buffer = bytearray()
        received_size = 0

        while True:

            base_changed.acquire()

            if self.nextseqnum < self.base + BROKER_WINDOW_SIZE:

                base_changed.release()

                # prepare packet 
                while received_size < PAYLOAD_SIZE: 
                
                    data = self.sConnectionSocket.recv(PAYLOAD_SIZE)# receive from s
                    
                    receive_buffer.extend(data)
                    received_size += len(data)

                payload = receive_buffer[:PAYLOAD_SIZE]
                receive_buffer = receive_buffer[PAYLOAD_SIZE:]
                received_size -= PAYLOAD_SIZE

                sequence_number = bytearray(str(self.nextseqnum).zfill(SEQUENCE_NUM_SIZE))

                intermediate = payload + sequence_number

                packet = intermediate + checksum(intermediate)

                # send to router 1
                self.r1Socket.sendto(send_data, (INTERFACE_3, PORT_3))

                # TODO handle two routers

                base_changed.acquire()

                if self.base == self.nextseqnum:

                    base_changed.release()

                    # TODO start timer

                base_changed.release()

                self.nextseqnum += 1

            else:
                self.base_changed.wait()

            base_changed.release()


            




#            if not window_buffer:
#
#                buffer_ctr = 0
#                
#                data = bytearray() 
#                
#                while buffer_ctr < BROKER_WINDOW_SIZE:
#                    
#                    get_data = self.sConnectionSocket.recv(PACKET_SIZE)# receive from s
#
#                    data.extend(get_data)
#                    
#                    #If the incoming data less than packet size, do not send it
#               
#                    if len(data) < PACKET_SIZE: 
#                        continue
#
#                    else:
#                        window_buffer.append(data)
#
#                        buffer_ctr += 1
#
#                        data = data[PACKET_SIZE:]
#
#            # else : no operation for receive
#
#            for idx in range(BROKER_WINDOW_SIZE):
#                send_data = window_buffer[idx] #Make sure to send data with only packet size
#            
#                self.r1Socket.sendto(send_data, (INTERFACE_3, PORT_3))
#
#            self.received = threading.Condition()
#
#            r1BackwardThread = threading.Thread(target=self.worker_r1)
#            r1BackwardThread.start()
#
#            # Timeout case
#            if not received.wait(TIMEOUT):
#                pass
#
#            else:
#                window_buffer = []

    def worker_receiver_1(self):

        receive_buffer = bytearray()
        received_size = 0

        while True:

            # prepare packet 
            while received_size < PACKET_SIZE: 
            
                data, _ = self.r1Socket.recvfrom(PACKET_SIZE)
                
                receive_buffer.extend(data)
                received_size += len(data)

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

            with self.base_changed:
    
                self.base = int(intsequence_number) + 1

                self.base_changed.notify()

            if self.base == self.nextseqnum:
                # TODO stop timer
            else:
                # TODO start timer

if __name__ == '__main__':
    BrokerNode().run()

