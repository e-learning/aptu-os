#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <arpa/inet.h>
#include <linux/ip.h>
#include <linux/icmp.h>


uint16_t checksum(uint16_t *buffer, int size) {
    ulong cksum = 0;
    while (size > 1) {
        cksum += *buffer++;
        size -= sizeof(uint16_t);
    }
    if (size)
        cksum += *(unsigned char *) buffer;
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >> 16);
    return (uint16_t) (~cksum);
}


int main(int argc, char **argv) {

    int sockfd;

    if(argc!=2)
    {
        perror("Usage: tracert [ip]\n");
        return EXIT_FAILURE;
    }

    if ((sockfd = socket (AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
    {
        perror("Could not create socket\n");
        return EXIT_FAILURE;
    }

    struct sockaddr_in addr;
    struct icmphdr icmp_hdr;
    addr.sin_family = AF_INET;
    inet_aton(argv[1],&addr.sin_addr);

    struct timeval timeout = {3, 0};

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval)) == -1) {
        perror("Setsockopt error\n");
        return EXIT_FAILURE;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(struct timeval)) == -1) {
        perror("setsockopt error\n");
        return EXIT_FAILURE;
    }

    icmp_hdr.type = ICMP_ECHO;
    icmp_hdr.code = 0;
    icmp_hdr.checksum = 0;
    icmp_hdr.un.echo.id = (__be16)(getpid() & 0xFFFF);
    icmp_hdr.un.echo.sequence = 0;

    char buf[1024];
    struct sockaddr_in rcv;
    socklen_t size = sizeof(rcv);

    bool done = false;
    for(int ttl = 1; ttl <= 255 & !done; ttl++) {

        setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof ttl);

        icmp_hdr.un.echo.sequence = (__be16)ttl;
        icmp_hdr.checksum = 0;
        icmp_hdr.checksum = checksum((uint16_t *)&icmp_hdr, sizeof icmp_hdr);

        if(sendto(sockfd, &icmp_hdr, sizeof(icmp_hdr), 0, (struct sockaddr *) &addr, sizeof(addr))==-1)
        {
            perror("Send error\n");
            return EXIT_FAILURE;
        }

        int bytes_read = recvfrom(sockfd, &buf, sizeof(buf), 0, (struct sockaddr *) &rcv, &size);
        struct iphdr *ip_hdr = (struct iphdr *) buf;
        int ip_header_len = ip_hdr->ihl * 4;

        if (bytes_read > 0) {
            icmphdr *icmpresp = (struct icmphdr *) (buf + ip_header_len);
            printf("%s\n", inet_ntoa(rcv.sin_addr));
            if(icmpresp->type == ICMP_ECHOREPLY || icmpresp->type == ICMP_DEST_UNREACH)
            {
                done = true;
            }
        }
        else {
            printf("* * *\n");
        }
    }

    close(sockfd);
    return 0;
}
