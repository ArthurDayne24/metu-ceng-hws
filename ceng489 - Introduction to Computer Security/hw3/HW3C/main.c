#include "commons.h"

void
run_task(int argc, char **argv)
{
    char *dev = argv[2];            /* capture device name */
    char errbuf[PCAP_ERRBUF_SIZE];  /* error buffer */
    pcap_t *handle;                 /* packet capture handle */

    char filter_exp[] = "ip";       /* filter expression [3] */
    struct bpf_program fp;          /* compiled filter program (expression) */
    bpf_u_int32 mask;               /* subnet mask */
    bpf_u_int32 net;                /* ip */

    /* get network number and mask associated with capture device */
    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        fprintf(stderr, "Couldn't get netmask for device %s: %s\n",
                dev, errbuf);
        error_exit(NULL);
    }

    /* print capture info */
    printf("Device: %s\n", dev);
    printf("Filter expression: %s\n", filter_exp);

    /* open capture device */
    handle = pcap_open_live(dev, SNAP_LEN, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
        error_exit(NULL);
    }

    /* make sure we're capturing on an Ethernet device [2] */
    if (pcap_datalink(handle) != DLT_EN10MB) {
        fprintf(stderr, "%s is not an Ethernet\n", dev);
        error_exit(NULL);
    }

    /* compile the filter expression */
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n",
                filter_exp, pcap_geterr(handle));
        error_exit(NULL);
    }

    /* apply the compiled filter */
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n",
                filter_exp, pcap_geterr(handle));
        error_exit(NULL);
    }

    /* now we can set our callback function */
    // task1
    if (strcmp(argv[1], "task1") == 0) {
        pcap_loop(handle, -1, got_packet_task1, NULL);
    }
    // task2
    else if (strcmp(argv[1], "task2") == 0) {
        char ips[100];
        memset(ips, 0, 100);
        strcpy(ips, argv[3]);
        strcpy(ips+50, argv[4]);
        pcap_loop(handle, -1, got_packet_task2, (u_char *) ips);
    }
    // task3
    else {
        pcap_loop(handle, -1, got_packet_task3, NULL);
    }

    /* cleanup */
    pcap_freecode(&fp);
    pcap_close(handle);
}

/*
 * arguments can be:
 * task1 <ethernet>
 * task2 <ethernet> <icmp_ip1> <icmp_ip2>
 * task3 <ethernet>
 */
int main(int argc, char **argv)
{
    // task1 or task2
    if ((argc == 3 && strcmp(argv[1], "task1") == 0) ||
        (argc == 5 && strcmp(argv[1], "task2") == 0) ||
        (argc == 3 && strcmp(argv[1], "task3") == 0)) {

        run_task(argc, argv);
    }
    else {
        error_exit("Wrong arguments are given.");
    }

    return 0;
}
