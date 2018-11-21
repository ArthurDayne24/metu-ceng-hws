
HEADER_SIZE   = 4
MAX_FILE_SIZE = 1024

ACK_MESSAGE  = "1" 
NACK_MESSAGE = "0"

INTERFACE_B   = "10.10.1.2" # interface-1
INTERFACE_R1  = "10.10.2.1" # interface-2
INTERFACE_R2  = "10.10.4.1" # interface-6
INTERFACE_D_1 = "10.10.3.2" # interface-5
INTERFACE_D_2 = "10.10.5.2" # interface-9

PORT_B   = 8090
PORT_R1  = 8090
PORT_R2  = 8090
PORT_D_1 = 8090
PORT_D_2 = 8090

def get_int_from_binary(byte):
    return int(byte.decode('utf-8'))

def get_binary_from_string(string):
    return bytearray(string, 'utf-8')

