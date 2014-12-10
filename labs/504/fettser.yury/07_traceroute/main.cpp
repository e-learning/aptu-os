#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <iostream>

using namespace std;

u_int16_t checksum(u_int16_t *buffer, int size)
{
    ulong cksum = 0;
    while (size > 1)
    {
        cksum += *buffer++;
        size -= sizeof(u_int16_t);
    }
    if (size)
    {
        cksum += *(unsigned char *) buffer;
    }
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >> 16);
    return (u_int16_t) (~cksum);
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        cout << "Usage: traceroute [Destination IP]" << endl;
        return -1;
    }

    int rawSocket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (rawSocket < 0)
    {
        perror("Can't create socket");
        return -1;
    }

    struct timeval timeout = {3, 0};

    struct sockaddr_in addr;
    struct icmphdr icmp_hdr;
    addr.sin_family = AF_INET;
    inet_aton(argv[1],&addr.sin_addr);

    if (setsockopt(rawSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval)) == -1)
    {
        perror("setsockopt error\n");
        return -1;
    }

    if (setsockopt(rawSocket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(struct timeval)) == -1)
    {
        perror("setsockopt error\n");
        return -1;
    }

    icmp_hdr.type = ICMP_ECHO;
    icmp_hdr.code = 0;
    icmp_hdr.checksum = 0;
    icmp_hdr.un.echo.id = (__be16)(getpid() & 0xFFFF);
    icmp_hdr.un.echo.sequence = 0;

    char buf[1024];
    struct sockaddr_in rcv;
    socklen_t rcv_size = sizeof(rcv);

    for (int ttl = 1, done = 0; ttl <= 255 && !done; ttl++)
    {
        setsockopt(rawSocket, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));

        icmp_hdr.un.echo.sequence = (__be16)ttl;
        icmp_hdr.checksum = 0;
        icmp_hdr.checksum = checksum((u_int16_t *)&icmp_hdr, sizeof icmp_hdr);

        if (sendto(rawSocket, &icmp_hdr, sizeof(icmp_hdr), 0, (struct sockaddr *) &addr, sizeof(addr)) == -1)
        {
            perror("Sending error\n");
            return -1;
        }

        int bytes_read = recvfrom(rawSocket, &buf, sizeof(buf), 0, (struct sockaddr *) &rcv, &rcv_size);
        struct iphdr *ip_hdr = (struct iphdr *) buf;
        int ip_header_len = ip_hdr->ihl * 4;

        if (bytes_read > 0)
        {
            icmphdr *icmpresp = (struct icmphdr *) (buf + ip_header_len);
            cout << inet_ntoa(rcv.sin_addr) << endl;
            if(icmpresp->type == ICMP_ECHOREPLY || icmpresp->type == ICMP_DEST_UNREACH)
            {
                done = true;
            }
        }
        else
        {
            cout << "* * *" << endl;
        }
    }

    close(rawSocket);
    return 0;
}
