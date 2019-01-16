#include "commons.h"

/*
 * task1 packet sniff callback
 */
void
got_packet_task1(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {

    /* packet counter */
    static long long unsigned count = 1;

    /* declare pointers to packet headers */
    /* The IP header */
    const struct sniff_ip *ip;

    int size_ip;

    printf("\nPacket number %llu:\n", count);
    count++;

    /* define/compute ip header offset */
    ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
    size_ip = IP_HL(ip)*4;
    if (size_ip < 20) {
        printf("   * Invalid IP header length: %u bytes, skipped\n", size_ip);
        return;
    }

    /* print source and destination IP addresses */
    printf("       From: %s\n", inet_ntoa(ip->ip_src));
    printf("         To: %s\n", inet_ntoa(ip->ip_dst));
}

/*
 * task2 packet sniff callback
 */
void
got_packet_task2(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {

    static long long unsigned count = 1;                   /* packet counter */

    /* declare pointers to packet headers */
    const struct sniff_ip *ip;              /* The IP header */
    const char *payload;                    /* Packet payload */

    int size_ip;
    int size_tcp;
    int size_payload;

    printf("\nPacket number %llu:\n", count);
    count++;

    /* define/compute ip header offset */
    ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
    size_ip = IP_HL(ip)*4;
    if (size_ip < 20) {
        printf("   * Invalid IP header length: %u bytes\n", size_ip);
        return;
    }

    /* determine protocol */
    switch(ip->ip_p) {
        case IPPROTO_TCP:
            break;
        case IPPROTO_ICMP:
            return;
        default:
            printf("   Protocol: unknown\n");
            return;
    }

    if (ip->ip_p == IPPROTO_TCP) {
        // TCP

        printf("   Protocol: TCP\n");

        /* print source and destination IP addresses */
        printf("       From: %s\n", inet_ntoa(ip->ip_src));
        printf("         To: %s\n", inet_ntoa(ip->ip_dst));

        /* define/compute tcp header offset */
        const struct sniff_tcp *tcp = (struct sniff_tcp *) (packet + SIZE_ETHERNET + size_ip);
        size_tcp = TH_OFF(tcp) * 4;
        if (size_tcp < 20) {
            printf("   * Invalid TCP header length: %u bytes, skipped\n", size_tcp);
            return;
        }

        uint16_t sport = ntohs(tcp->th_sport);
        uint16_t dport = ntohs(tcp->th_dport);

        if (dport >= 10 && dport <= 100) {

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

        printf("   Protocol: ICMP\n");

        struct in_addr sip = ip->ip_src;
        struct in_addr dip = ip->ip_dst;

        // TODO implement input mechanism
        struct in_addr desired_dip;
        if (dip.s_addr == desired_dip.s_addr) {

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

void task1_2(int argc, char **argv)
{

    char *dev = NULL;               /* capture device name */
    char errbuf[PCAP_ERRBUF_SIZE];  /* error buffer */
    pcap_t *handle;                 /* packet capture handle */

    char filter_exp[] = "ip";       /* filter expression [3] */
    struct bpf_program fp;          /* compiled filter program (expression) */
    bpf_u_int32 mask;               /* subnet mask */
    bpf_u_int32 net;                /* ip */

    dev = argv[2];

    /* get network number and mask associated with capture device */
    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        fprintf(stderr, "Couldn't get netmask for device %s: %s\n",
                dev, errbuf);
        net = 0;
        mask = 0;
    }

    /* print capture info */
    printf("Device: %s\n", dev);
    printf("Filter expression: %s\n", filter_exp);

    /* open capture device */
    handle = pcap_open_live(dev, SNAP_LEN, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
        exit(EXIT_FAILURE);
    }

    /* make sure we're capturing on an Ethernet device [2] */
    if (pcap_datalink(handle) != DLT_EN10MB) {
        fprintf(stderr, "%s is not an Ethernet\n", dev);
        exit(EXIT_FAILURE);
    }

    /* compile the filter expression */
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n",
                filter_exp, pcap_geterr(handle));
        exit(EXIT_FAILURE);
    }

    /* apply the compiled filter */
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n",
                filter_exp, pcap_geterr(handle));
        exit(EXIT_FAILURE);
    }

    /* now we can set our callback function */
    // task1
    if (argc == 3) {
        pcap_loop(handle, -1, got_packet_task1, NULL);
    }
        // task2
    else {
        pcap_loop(handle, -1, got_packet_task2, NULL);
    }

    /* cleanup */
    pcap_freecode(&fp);
    pcap_close(handle);
}