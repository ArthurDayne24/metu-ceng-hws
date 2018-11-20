import socket
import sys

# TODO check response messages ACK / NACK and connection close message

HEADER_SIZE = 4
ACK_MESSAGE = "1"
NACK_MESSAGE = "0"
MAX_FILE_SIZE = 1000
STRING = "hello world xd xd 122458?**hello world xd xd 122458?**hello world xd xd 122458?**"

def get_binary_from_int(integer):
    return bytearray(str(integer), 'utf-8')

def get_binary_from_string(string):
    return bytearray(string, 'utf-8')

def read_file():
    return get_binary_from_string(STRING)

def main():
    port = int(sys.argv[1])

    clientsocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    clientsocket.connect(('localhost', port))
    
    # send max size of a file
    clientsocket.send(get_binary_from_int(MAX_FILE_SIZE))
    # get ack
    clientsocket.recv(HEADER_SIZE) 

    while True:
        # send each file / chunk

        file_stream = read_file()
        
        # send file
        clientsocket.send(file_stream)
         # get ack
        clientsocket.recv(HEADER_SIZE)

if __name__ == '__main__':
    main()

