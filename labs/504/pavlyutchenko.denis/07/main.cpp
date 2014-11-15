#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <unistd.h>
#include <string>
#include <iostream>


unsigned short in_cksum(unsigned short *addr, int len)
{
    register int sum = 0;
    u_short answer = 0;
    register u_short *w = addr;
    register int nleft = len;

    while (nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }

    if (nleft == 1)
    {
        *(u_char *) (&answer) = *(u_char *) w;
        sum += answer;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return (answer);
}

std::string ping(std::string ip_adress, __u8 ttl, unsigned int timeout)
{
    int addrlen;
    int k;

    struct iphdr *ip;
    struct icmphdr *icmp;
//    struct icmphdr *icmp_reply;
    struct iphdr *ipreply;

    char *packet;
    char *buffer;
    int raw;
    struct sockaddr_in serv;
    int optval=1;
    int sent;

    packet = new char[sizeof(struct iphdr) + sizeof(struct icmphdr)];
    buffer = new char[sizeof(struct iphdr) + sizeof(struct icmphdr)];

    ip = (struct iphdr *)packet;

    ip->ihl = 5;  //header length
    ip->version = 4; //version
    ip->tos = 0;    //type of service
    ip->tot_len = htons(sizeof(struct iphdr)+sizeof(struct icmphdr)); //total length
    ip->id = htons((uint16_t) random()); //Assign id as a random number
    ip->ttl = ttl; //time to live;
    ip->protocol = IPPROTO_ICMP; //ICMP header follow next to the ip header
    ip->check = 0; //Assign checksum as zero for now
    ip->saddr = INADDR_ANY;
    ip->daddr=inet_addr(ip_adress.c_str());

    if((raw = socket(AF_INET, SOCK_RAW,IPPROTO_ICMP)) == -1)
    {
        perror("socket call error.");
    }

    setsockopt(raw, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(int));

    icmp = (struct icmphdr *)(packet+sizeof(struct iphdr));

    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->un.echo.id = 0;
    icmp->un.echo.sequence = 0;
    icmp->checksum = 0;
    icmp-> checksum = in_cksum((unsigned short *)icmp, sizeof(struct icmphdr));

    ip->check = in_cksum((unsigned short *)ip, sizeof(struct iphdr));

    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = ip->daddr;

    sent = (int) sendto(raw, packet, ntohs(ip->tot_len), 0, (struct sockaddr *)&serv, sizeof(serv));

    if (sent)
    {

    }

    struct timeval timeval1;
    timeval1.tv_sec = timeout;
    timeval1.tv_usec = 0;

    if (setsockopt (raw, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeval1, sizeof(timeval1)) < 0)
    {
        printf("setsockopt failed\n");
    }

    if (setsockopt (raw, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeval1, sizeof(timeval1)) < 0)
    {
        printf("setsockopt failed\n");
    }

    addrlen = sizeof(serv);
    if ((k= (int) recvfrom(raw, buffer, (sizeof(struct iphdr) + sizeof(struct icmphdr)), 0, (struct sockaddr *)&serv, (socklen_t *) &addrlen)) == -1)
    {
        return "***";
    }

    ipreply=(struct iphdr *)buffer;
//    icmp_reply=(struct icmphdr *)(buffer + sizeof (struct iphdr));

    struct in_addr inad;
    inad.s_addr = ipreply->saddr;

    close(raw);
    free(packet);

    return inet_ntoa (inad);
}

bool validate_ip_address(const std::string &ipAddress)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress.c_str(), &(sa.sin_addr));
    return result != 0;
}

int main(int argc, char** argv)
{
    const int max_ttl = 32;
    const int max_timeout = 3;
    std::string ip = "";

    if (getuid())
    {
        printf("%s", "You must be root!\n");
        return 1;
    }

    if ( (argc <= 1) || (argv[argc-1] == NULL) || (argv[argc-1][0] == '-') ) {
        std::cout << "No argument provided!" << std::endl;
        return 1;
    }
    else {
        ip = argv[1];
    }

    if (!validate_ip_address(ip))
    {
        std::cout << "There is no host: " + ip << std::endl;
        return 1;
    }

    for (__u8 current_ttl = 1; current_ttl < max_ttl; ++current_ttl)
    {
        std::string ip_result = ping(ip, current_ttl, max_timeout);

        std::cout << ip_result << std::endl;

        if (ip_result == ip)
        {
            break;
        }
    }
}