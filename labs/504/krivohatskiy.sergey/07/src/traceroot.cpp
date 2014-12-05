#include "traceroot.h"
#include <exception>
#include <linux/icmp.h>
#include <linux/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <memory.h>

// from http://www.microhowto.info/howto/calculate_an_internet_protocol_checksum_in_c.html
uint16_t ip_checksum(void* vdata,size_t length) {
    // Cast the data pointer to one that can be indexed.
    char* data=(char*)vdata;

    // Initialise the accumulator.
    uint32_t acc=0xffff;

    // Handle complete 16-bit blocks.
    for (size_t i=0;i+1<length;i+=2) {
        uint16_t word;
        memcpy(&word,data+i,2);
        acc+=ntohs(word);
        if (acc>0xffff) {
            acc-=0xffff;
        }
    }

    // Handle any partial block at the end of the data.
    if (length&1) {
        uint16_t word=0;
        memcpy(&word,data+length-1,1);
        acc+=ntohs(word);
        if (acc>0xffff) {
            acc-=0xffff;
        }
    }

    // Return the checksum in network byte order.
    return htons(~acc);
}

bool validate_address(const std::string &ipAddress)
{
    sockaddr_in sa;
    // returns 0 if ipAddress is not a valid ip address
    return inet_pton(AF_INET, ipAddress.c_str(), &(sa.sin_addr));
}

void fill_packet(char *packet, const char *address, int ttl)
{
    iphdr *ip = (iphdr *)packet;
    memset(ip, 0, sizeof(iphdr));
    ip->ihl = 5;
    ip->version = 4;
    ip->tot_len = htons(sizeof(iphdr)+sizeof(icmphdr));
    ip->id = htons(123);
    ip->ttl = ttl;
    ip->protocol = IPPROTO_ICMP;
    ip->saddr = INADDR_ANY;
    ip->daddr = inet_addr(address);
    ip->check = ip_checksum(packet, sizeof(iphdr));

    icmphdr *icmp = (icmphdr *)(packet+sizeof(iphdr));
    memset(icmp, 0, sizeof(icmphdr));
    icmp->type = ICMP_ECHO;
    icmp->checksum = ip_checksum((unsigned short *)icmp, sizeof(icmphdr));
}

void traceroot::print(std::ostream &out, const std::string &dest_ip)
{
    out << "traceroot to " << dest_ip << ":\n";

    if(!validate_address(dest_ip))
    {
        throw traceroot_exception(dest_ip + " is not a valid ip address");
    }

    for(int i = 1; i <= MAX_TTL; ++i)
    {
        std::string ip = do_hop(i, dest_ip);
        out << ip << std::endl;
        if(ip == dest_ip)
        {
            return;
        }
    }
    throw traceroot_exception("MAX_TTL limit exeeded");
}
std::string traceroot::do_hop(int ttl, const std::string &ip_adress)
{
    char packet[sizeof(iphdr) + sizeof(icmphdr)];
    fill_packet(packet, ip_adress.c_str(), ttl);

    iphdr *ip = (iphdr *)packet;

    sockaddr_in serv;
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = ip->daddr;

    // create socket
    int raw_socket = socket(AF_INET, SOCK_RAW,IPPROTO_ICMP);
    if(raw_socket == -1)
    {
        throw traceroot_exception("socket()");
    }

    // set option header is included with data = false
    int val = 1;
    if(setsockopt(raw_socket, IPPROTO_IP, IP_HDRINCL, &val, sizeof(int)) < 0)
    {
        close(raw_socket);
        throw traceroot_exception("setsockopt()");
    }

    // send packet
    if ((int) sendto(raw_socket, packet, ntohs(ip->tot_len), 0, (sockaddr *)&serv, sizeof(serv)) == -1)
    {
        close(raw_socket);
        throw traceroot_exception("sendto()");
    }

    // set timeouts for send and receive
    timeval timeval1;
    timeval1.tv_sec = MAX_TIMEOUT;
    timeval1.tv_usec = 0;
    if (setsockopt (raw_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeval1, sizeof(timeval1)) < 0)
    {
        close(raw_socket);
        throw traceroot_exception("setsockopt()");
    }

    if (setsockopt (raw_socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeval1, sizeof(timeval1)) < 0)
    {
        close(raw_socket);
        throw traceroot_exception("setsockopt()");
    }

    // receive
    int addrlen = sizeof(serv);
    char buffer[sizeof(iphdr) + sizeof(icmphdr)];
    if ((int) recvfrom(raw_socket, buffer, (sizeof(iphdr) + sizeof(icmphdr)), 0, (sockaddr *)&serv, (socklen_t *) &addrlen) == -1)
    {
        close(raw_socket);
        return "------------";
    }
    close(raw_socket);


    iphdr *ipreply = (iphdr *)buffer;

    in_addr inad;
    inad.s_addr = ipreply->saddr;


    return std::string(inet_ntoa(inad));

}
