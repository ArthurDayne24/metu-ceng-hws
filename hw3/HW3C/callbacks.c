#include "commons.h"

/*
 * task1 packet sniff callback
 */
void
got_packet_task1(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {

    /* packet counter */
    static long long unsigned count = 1;

    /* The IP header */
    const struct sniff_ip *ip;

    int size_ip;

    /* define/compute ip header offset */
    ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
    size_ip = IP_HL(ip)*4;
    if (size_ip < 20) {
        printf("   * Invalid IP header length: %u bytes, skipped.\n", size_ip);
        return;
    }

    printf("\nPacket number %llu:\n", count++);

    /* print source and destination IP addresses */
    printf("       From: %s\n", inet_ntoa(ip->ip_src));
    printf("         To: %s\n", inet_ntoa(ip->ip_dst));
}

/*
 * task2 packet sniff callback
 */
void
got_packet_task2(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {

    /* packet counter */
    static long long unsigned count = 1;

    /* The IP header */
    const struct sniff_ip *ip;
    /* Packet payload */
    const char *payload;

    int size_ip;
    int size_tcp;
    int size_payload;

    /* define/compute ip header offset */
    ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
    size_ip = IP_HL(ip)*4;
    if (size_ip < 20) {
        printf("   * Invalid IP header length: %u bytes, skipped.\n", size_ip);
        return;
    }

    /* determine protocol */
    switch(ip->ip_p) {
        case IPPROTO_TCP:
            break;
        case IPPROTO_ICMP:
            break;
        default:
            return;
    }

    if (ip->ip_p == IPPROTO_TCP) {
        // TCP

        /* define/compute tcp header offset */
        const struct sniff_tcp *tcp = (struct sniff_tcp *) (packet + SIZE_ETHERNET + size_ip);
        size_tcp = TH_OFF(tcp) * 4;
        if (size_tcp < 20) {
            printf("   * Invalid TCP header length: %u bytes, skipped.\n", size_tcp);
            return;
        }

        uint16_t sport = ntohs(tcp->th_sport);
        uint16_t dport = ntohs(tcp->th_dport);

        if (dport >= 10 && dport <= 100) {

            printf("\nPacket number %llu:\n", count++);

            printf("   Protocol: TCP\n");

            /* print source and destination IP addresses */
            printf("       From: %s\n", inet_ntoa(ip->ip_src));
            printf("         To: %s\n", inet_ntoa(ip->ip_dst));

            printf("   Src port: %d\n", sport);
            printf("   Dst port: %d\n", dport);

            /* define/compute tcp payload (segment) offset */
            payload = (const char *) (u_char *) (packet + SIZE_ETHERNET + size_ip + size_tcp);

            /* compute tcp payload (segment) size */
            size_payload = ntohs(ip->ip_len) - (size_ip + size_tcp);

            /*
             * Print payload data; it might be binary, so don't just
             * treat it as a string.
             */
            if (size_payload > 0) {
                printf("   Payload (%d bytes):\n", size_payload);
                print_payload((const u_char *) payload, size_payload);
            }
        }
    }
    else {
        // ICMP

        struct in_addr sip = ip->ip_src;
        struct in_addr dip = ip->ip_dst;

        // convert desired ip address (string) to s_addr form
        struct in_addr host1_ip;
        inet_pton(AF_INET, (const char *) args, &host1_ip.s_addr);
        struct in_addr host2_ip;
        inet_pton(AF_INET, (const char *) (args+50), &host2_ip.s_addr);

        if ((dip.s_addr == host1_ip.s_addr && sip.s_addr == host2_ip.s_addr) || (dip.s_addr == host2_ip.s_addr && sip.s_addr == host1_ip.s_addr)) {

            printf("\nPacket number %llu:\n", count++);

            printf("   Protocol: ICMP\n");

            /* print source and destination IP addresses */
            printf("       From: %s\n", inet_ntoa(sip));
            printf("         To: %s\n", inet_ntoa(dip));

            const struct sniff_icmp *icmp = (struct sniff_icmp *) (packet + SIZE_ETHERNET + size_ip);

            printf("       Type: %d\n", icmp->ic_type);
            printf("       Code: %d\n", icmp->ic_code);
            printf("         Id: %d\n", icmp->ic_id);
            printf("        Seq: %d\n", icmp->ic_seq);
        }
    }
}

/*
 * task3 packet sniff callback
 */
void
got_packet_task3(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {

    /* packet counter */
    static long long unsigned count = 1;

    /* The IP header */
    const struct sniff_ip *ip;
    /* Packet payload */
    const char *payload;

    int size_ip;
    int size_tcp;
    int size_payload;

    /* define/compute ip header offset */
    ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
    size_ip = IP_HL(ip)*4;
    if (size_ip < 20) {
        printf("   * Invalid IP header length: %u bytes, skipped.\n", size_ip);
        return;
    }

    if (ip->ip_p == IPPROTO_TCP) {
        // TCP

        /* define/compute tcp header offset */
        const struct sniff_tcp *tcp = (struct sniff_tcp *) (packet + SIZE_ETHERNET + size_ip);
        size_tcp = TH_OFF(tcp) * 4;
        if (size_tcp < 20) {
            printf("   * Invalid TCP header length: %u bytes, skipped.\n", size_tcp);
            return;
        }

        uint16_t sport = ntohs(tcp->th_sport);
        uint16_t dport = ntohs(tcp->th_dport);

        // TODO how to identify if it is telnet ?
        printf("\nPacket number %llu:\n", count++);

        printf("   Protocol: TCP\n");

        /* print source and destination IP addresses */
        printf("       From: %s\n", inet_ntoa(ip->ip_src));
        printf("         To: %s\n", inet_ntoa(ip->ip_dst));

        printf("   Src port: %d\n", sport);
        printf("   Dst port: %d\n", dport);

        /* define/compute tcp payload (segment) offset */
        payload = (const char *) (u_char *) (packet + SIZE_ETHERNET + size_ip + size_tcp);

        /* compute tcp payload (segment) size */
        size_payload = ntohs(ip->ip_len) - (size_ip + size_tcp);

        /*
         * Print payload data; it might be binary, so don't just
         * treat it as a string.
         */
        if (size_payload > 0) {
            printf("   Payload (%d bytes):\n", size_payload);
            print_payload((const u_char *) payload, size_payload);
        }
    }
}
