#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/ip_icmp.h>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <limits>

using namespace std;

void usage()
{
    cout << "Usage: traceroute IP\n";
    cout << "IP is IPv4 address in dot-decimal notation";
    exit(-1);
}

char buf[512];

uint16_t icmp_cksum(char *msg, int length)
{
    uint32_t acc = numeric_limits<uint16_t>::max();

    for (int i = 0; i < length - 1; i += 2)
    {
        acc += ntohs(*reinterpret_cast<uint16_t*>(msg + i));

    }
    if (length % 2)
    {
       acc += (uint8_t)msg[length - 1];
    }

    acc %= numeric_limits<uint16_t>::max();

    return htons(~acc);
}

icmp* echo_header(int id)
{
    memset(buf, 0, 512);
    icmp *ret = (icmp*)buf;
    ret->icmp_type = ICMP_ECHO;
    ret->icmp_code = static_cast<uint8_t>(htons(0));
    ret->icmp_seq = id;
    ret->icmp_id  = 111;
    ret->icmp_cksum = icmp_cksum(buf, sizeof(icmp));
    return ret;
}

int main(int argc, const char **argv)
{
    if (argc != 2)
    {
        usage();
    }

    sockaddr_in host_addr;
    memset(&host_addr, 0, sizeof(host_addr));
    host_addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, argv[1], &host_addr.sin_addr) != 1)
    {
        usage();
    }

    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    if (sock == -1)
    {
        perror("Error on socket creation");
        return -1;
    }

    timeval tv;

    tv.tv_sec = 10;
    tv.tv_usec = 0;

    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(timeval)) == -1)
    {
        perror("Error on set timeout");
    }

    if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&tv, sizeof(timeval)) == -1)
    {
        perror("Error on set timeout");
    }

    char response[512];

    vector<in_addr> route;
    for (int ttl = 1; ttl <= 255; ++ttl)
    {
        if (setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) == -1)
        {
            perror("Error on set TTL");
            return -1;
        }
        echo_header(ttl);
        if (sendto(sock, buf, 8, 0, (sockaddr*)&host_addr, sizeof(host_addr)) == -1)
        {
            perror("Error on sending request");
            return -1;
        }

        bool fail = false;

        int bytes_received = recvfrom(sock, &response, 512, 0, 0, 0);
        if (bytes_received == -1)
        {
            fail = true;
            perror("Error on recvfrom");
        }

        ip *response_ip_s = (ip*)response;

        icmp *response_icmp = (icmp*)(response + response_ip_s->ip_hl * 4);
        if (!fail && response_icmp->icmp_type != ICMP_ECHOREPLY && response_icmp->icmp_code != ICMP_EXC_TTL)
        {
            fail = true;
            cerr << "Wrong response" << endl;
        }

        if (!fail)
        {
            cout << inet_ntoa(response_ip_s->ip_src) << endl;
            if (response_icmp->icmp_type == ICMP_ECHOREPLY)
                exit(0);
        }

        if (fail)
        {
            cout << "no reply" << endl;
        }
    }
    return 0;
}

