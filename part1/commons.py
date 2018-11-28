
ACK_SIZE = 1 #size of acknowledgement
PACKET_SIZE = 32 #size of the sensor reading message

ACK_MESSAGE  = "1" 
NACK_MESSAGE = "0" #this is not used in part 1

#Interfaces
INTERFACE_1 = "10.10.1.2"
INTERFACE_2 = "10.10.2.1"
INTERFACE_3 = "10.10.2.2"
INTERFACE_4 = "10.10.3.1"
INTERFACE_5 = "10.10.3.2"
INTERFACE_6 = "10.10.4.1"
INTERFACE_7 = "10.10.4.2"
INTERFACE_8 = "10.10.5.1"
INTERFACE_9 = "10.10.5.2"

PORT = 8100 #We use the same port throughout the network

#converts bytearray to int
def get_int_from_binary(byte):
    return int(byte.decode('utf-8'))

#converts string to bytearray
def get_binary_from_string(string):
    return bytearray(string, 'utf-8')

