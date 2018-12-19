
ON_LOCAL = True

TIMEOUT = 1 # seconds

TOTAL_BYTES = 5 * 1024 * 1024

CHECKSUM_SIZE = 16
SEQUENCE_NUM_SIZE = 8

HEADER_SIZE = CHECKSUM_SIZE + SEQUENCE_NUM_SIZE

PAYLOAD_SIZE = 16

PACKET_SIZE = PAYLOAD_SIZE + HEADER_SIZE

# Packet Format:
# < PAYLOAD | SEQUENCE | CHECKSUM >
# CHECKSUM is computed over < PAYLOAD | SEQUENCE >
# Then concataneted

ACK_MESSAGE  = "1" 
NACK_MESSAGE = "0" #this is not used in part 1

# Interfaces
if ON_LOCAL:
    INTERFACE_1 = "localhost"
    INTERFACE_2 = "localhost"
    INTERFACE_3 = "localhost"
    INTERFACE_4 = "localhost"
    INTERFACE_5 = "localhost"
    INTERFACE_6 = "localhost"
    INTERFACE_7 = "localhost"
    INTERFACE_8 = "localhost"
    INTERFACE_9 = "localhost"

    PORT_1 = 8101
    PORT_2 = 8102
    PORT_3 = 8103
    PORT_4 = 8104
    PORT_5 = 8105
    PORT_6 = 8106
    PORT_7 = 8107
    PORT_8 = 8108
    PORT_9 = 8109

else:
    INTERFACE_1 = "10.10.1.2"
    INTERFACE_2 = "10.10.2.1"
    INTERFACE_3 = "10.10.2.2"
    INTERFACE_4 = "10.10.3.1"
    INTERFACE_5 = "10.10.3.2"
    INTERFACE_6 = "10.10.4.1"
    INTERFACE_7 = "10.10.4.2"
    INTERFACE_8 = "10.10.5.1"
    INTERFACE_9 = "10.10.5.2"

    __COMMON_PORT = 8100
    
    # We use the same port throughout the network
    PORT_1 = __COMMON_PORT 
    PORT_2 = __COMMON_PORT 
    PORT_3 = __COMMON_PORT 
    PORT_4 = __COMMON_PORT 
    PORT_5 = __COMMON_PORT 
    PORT_6 = __COMMON_PORT 
    PORT_7 = __COMMON_PORT 
    PORT_8 = __COMMON_PORT 
    PORT_9 = __COMMON_PORT 

BROKER_WINDOW_SIZE = 40

#converts bytearray to int
def get_int_from_binary(byte):
    return int(byte.decode('utf-8'))

#converts string to bytearray
def get_binary_from_string(string):
    return bytearray(string, 'utf-8')

def checksum(payload_sequence):
    first_byte  = sum(payload_sequence[i] for i in range(0, PAYLOAD_SIZE + SEQUENCE_NUM_SIZE, 2))
    second_byte = sum(payload_sequence[i+1] for i in range(0, PAYLOAD_SIZE + SEQUENCE_NUM_SIZE, 2))

    first_byte += (second_byte % 0xFF) >> 8
    second_byte = (second_byte - second_byte % 0xFF)

    first_byte  = chr(first_byte)
    second_byte = chr(second_byte)

    return bytearray(first_byte + second_byte)

