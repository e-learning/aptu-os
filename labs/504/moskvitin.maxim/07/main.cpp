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



using namespace std;

void usage()
{
    cout << "Usage: traceroute IP\n";
    cout << "IP is IPv4 address in dot-decimal notation";
    exit(-1);
}

char buf[512];

icmp* echo_header()
{
    memset(buf, 0, 512);
    icmp *ret = (icmp*)buf;
    ret->icmp_type = ICMP_ECHO;
    ret->icmp_code = static_cast<uint8_t>(htons(0));
    ret->icmp_cksum = 65527;
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

    char response[512];

    vector<in_addr> route;
    for (int ttl = 1; ttl <= 255; ++ttl)
    {
        if (setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) == -1)
        {
            perror("Error on set TTL");
            return -1;
        }
        echo_header();
        if (sendto(sock, buf, 8, 0, (sockaddr*)&host_addr, sizeof(host_addr)) == -1)
        {
            perror("Error on sending request");
            return -1;
        }
        int fails = 0;
        while (fails < 100)
        {
            int bytes_received = recvfrom(sock, &response, 512, 0, 0, 0);
            if (bytes_received == -1)
            {
                ++fails;
                continue;
            }

            ip *response_ip_s = (ip*)response;

            if (response_ip_s->ip_p != IPPROTO_ICMP)
            {
                ++fails;
                continue;
            }

            icmp *response_icmp = (icmp*)(response + response_ip_s->ip_hl * 4);
            if (response_icmp->icmp_type != ICMP_ECHOREPLY && response_icmp->icmp_code != ICMP_EXC_TTL)
            {
                ++fails;
                continue;
            }

            cout << inet_ntoa(response_ip_s->ip_src) << endl;
            if (response_icmp->icmp_type == ICMP_ECHOREPLY)
                exit(0);
            break;
        }

        if (fails >= 100)
        {
            cout << "no reply" << endl;
        }
    }
    return 0;
}

