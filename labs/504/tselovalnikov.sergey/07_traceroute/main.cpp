#include <stdio.h>
#include <stdlib.h>   
#include <string.h>
#include <sys/un.h>   
#include <unistd.h>
#include <netinet/in.h>   
#include <netdb.h>
#include <arpa/inet.h>
#include <linux/ip.h>
#include <linux/icmp.h>


#define MAX_HOPS 30
#define ICMP_MIN 8

#define DEF_PACKET_SIZE  32
#define MAX_PACKET_SIZE  1024

typedef struct icmphdr ICMP_HDR;

void print_usage() {
    printf("usage ./07_traceroute host-name [max-hops]\n");
    exit(-1);
}

int handle_response(char *buf, int bytes, struct sockaddr_in *from, int ttl) {
    struct iphdr *ip_hdr = (struct iphdr *) buf;
    int ip_header_len = ip_hdr->ihl * 4;

    if (bytes < ip_header_len + ICMP_MIN) {
        return 0;
    }

    struct in_addr inaddr = from->sin_addr;
    ICMP_HDR *icmphdr = (ICMP_HDR *) (buf + ip_header_len);
    struct hostent *lpHostent;
    __u8 header_type = icmphdr->type;
    if (header_type == ICMP_ECHOREPLY || header_type == ICMP_TIME_EXCEEDED) {
        lpHostent = gethostbyaddr((const char *) &from->sin_addr, 4, AF_INET);
        if (lpHostent)
            printf("%2d  %s (%s)\n", ttl, lpHostent->h_name, inet_ntoa(inaddr));
        else
            printf("%2d  %s\n", ttl, inet_ntoa(inaddr));
        return header_type == ICMP_ECHOREPLY;
    }
    if (header_type == ICMP_DEST_UNREACH) {
        printf("%2d  %s  Host is unreachable\n", ttl, inet_ntoa(inaddr));
        return 0;
    } else {
        printf("undefined type %d recvd\n", header_type);
        return 1;
    }
}

ushort checksum(ushort *buffer, int size) {
    ulong cksum = 0;
    while (size > 1) {
        cksum += *buffer++;
        size -= sizeof(ushort);
    }
    if (size)
        cksum += *(unsigned char *) buffer;
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >> 16);
    return (ushort) (~cksum);
}

void fill_icmp_pack(char *icmp_data, int datasize) {
    ICMP_HDR *icmp_hdr = (ICMP_HDR *) icmp_data;
    icmp_hdr->type = ICMP_ECHO;
    icmp_hdr->code = 0;
    icmp_hdr->un.echo.id = (ushort) getpid();
    icmp_hdr->checksum = 0;
    icmp_hdr->un.echo.sequence = 0;

    char *datapart = icmp_data + sizeof(ICMP_HDR);
    memset(datapart, 'E', datasize - sizeof(ICMP_HDR));
}

int main(int argc, char **argv) {
    if (argc < 2) {
        print_usage();
    }

    int maxhops = argc == 3 ? atoi(argv[2]) : MAX_HOPS;

    int sockRaw = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockRaw < 0) {
        perror("socket error");
        return -1;
    }

    struct timeval timeout;
    timeout.tv_usec = 0;
    timeout.tv_sec = 3;

    int ret = setsockopt(sockRaw, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval));
    if (ret == -1) {
        perror("setsockopt error:");
        return -1;
    }

    ret = setsockopt(sockRaw, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(struct timeval));
    if (ret == -1) {
        perror("setsockopt error:");
        return -1;
    }

    struct sockaddr_in dest;
    memset(&dest, 0, sizeof(struct sockaddr_in));
    dest.sin_family = AF_INET;
    char *host_name = argv[1];
    if ((dest.sin_addr.s_addr = inet_addr(host_name)) == INADDR_NONE) {
        struct hostent *hp = gethostbyname(host_name);
        if (!hp) {
            printf("%s can not be resolved", host_name);
            return 0;
        }
        memcpy(&dest.sin_addr, hp->h_addr, hp->h_length);
    }
    int datasize = DEF_PACKET_SIZE + sizeof(ICMP_HDR);

    ICMP_HDR *icmp_data = (ICMP_HDR *) malloc(MAX_PACKET_SIZE * sizeof(char));
    char *recvbuf = (char *) malloc(MAX_PACKET_SIZE * sizeof(char));

    memset(icmp_data, 0, MAX_PACKET_SIZE);
    fill_icmp_pack((char *) icmp_data, datasize);

    printf("traceroute to %s, %d hops max:\n\n", host_name, maxhops);
    struct sockaddr_in from;
    socklen_t sock_len = sizeof(from);
    for (int ttl = 1, i = 0, done = 0; ttl < maxhops && !done; ttl++) {
        int nRet = setsockopt(sockRaw, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));
        if (nRet < 0) {
            perror("setsockopt error:");
        }

        icmp_data->checksum = 0;
        icmp_data->un.echo.sequence = i++;
        icmp_data->checksum = checksum((ushort *) icmp_data, datasize);

        int bwrote = sendto(sockRaw, icmp_data, datasize, 0, (struct sockaddr *) &dest, sizeof(dest));
        if (bwrote < 0) {
            perror("error: ");
            return -1;
        }

        int ret = recvfrom(sockRaw, recvbuf, MAX_PACKET_SIZE, 0, (struct sockaddr *) &from, &sock_len);
        if (ret < 0) {
            printf("%2d  * * *\n", ttl);
            continue;
        }

        done = handle_response(recvbuf, ret, &from, ttl);
    }
    free(recvbuf);
    free(icmp_data);
    return 0;
}   