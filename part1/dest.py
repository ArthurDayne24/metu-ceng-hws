import socket
import sys
import threading 
import time
import random

# TODO check response messages ACK / NACK and connection close message

HEADER_SIZE = 4
ACK_MESSAGE = "1" 
NACK_MESSAGE = "0"
MAX_CONNECTIONS = 1
BIND_IP = "10.10.3.2"

def get_int_from_binary(byte):
    return int(byte.decode('utf-8'))

def get_binary_from_string(string):
    return bytearray(string, 'utf-8')

def worker_connectionsocket(connectionsocket, clientid):
    # get max size of a file
    data = connectionsocket.recv(HEADER_SIZE)
    MAX_FILE_SIZE = get_int_from_binary(data)
    # send ack
    connectionsocket.send(get_binary_from_string(ACK_MESSAGE))

    while True:
        # get each file / chunk

        data = connectionsocket.recv(MAX_FILE_SIZE)
        print("Get data from", clientid)
        time.sleep(random.uniform(0.2, 1.0))
        connectionsocket.send(get_binary_from_string(ACK_MESSAGE))
        print("Sent data to", clientid)

def main():
    port = int(sys.argv[1])

    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.bind((BIND_IP, port))
    serversocket.listen(MAX_CONNECTIONS)
    
    nconnections = 0

    while nconnections < MAX_CONNECTIONS:
        nconnections += 1
        
        connectionsocket, address = serversocket.accept()
        print("Welcome Client nconnections", nconnections)

        thread = threading.Thread(target=worker_connectionsocket, 
                args=(connectionsocket, nconnections,))
        thread.start()

    serversocket.close()

if __name__ == '__main__':
    main()

