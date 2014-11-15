#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>


#define ICMP_HEADER_LEN 8
#define TIMEOUT 3000
#define TTL_LIMIT 50
#define BUFFER_SIZE 128

unsigned short in_cksum(unsigned short *addr, int len)
{
    int left = len;
    unsigned short *word = addr;
    unsigned short ans;
    unsigned sum = 0;

    while (left > 1) {
        sum += *word++;
        left -= 2;
    }

    if (left == 1) {
        unsigned short tail = 0;
        *(unsigned char *)(&tail) = *(unsigned char *)word;
        sum += tail;
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    ans = ~sum;

    return ans;
}

double time_diff(struct timeval start, struct timeval end)
{
    return (end.tv_sec - start.tv_sec)*1000.0 + (end.tv_usec - start.tv_usec)/1000.0;
}


int main(int argc, char* argv[])
{
    
    if(argc != 2)
    {
        printf("Usage: traceroute IP\n");
        return -1;
    }
    
    struct sockaddr_in remote_addr;
    bzero(&remote_addr, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, argv[1], &remote_addr.sin_addr) == 0)
    {
        fprintf(stderr, "Invalid IP address\n");
        return -1;
    }
      
    int pid = getpid(); 
    
    int socket_id = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (socket_id < 0)
    {
        fprintf(stderr, "socket() error: %s\n", strerror(errno));
        return -1;
    }
    
    struct timeval begin, current;
    begin.tv_sec = 0;
    begin.tv_usec = 1000; 
    
    if (setsockopt(socket_id, SOL_SOCKET, SO_RCVTIMEO, &begin, sizeof(begin)) < 0) 
    {
        fprintf(stderr, "setsockopt() error: %s\n", strerror(errno));
        return -1;
    }
    
    char icmp_request_buff[BUFFER_SIZE], reply_buff[BUFFER_SIZE];  
    
    struct icmp *icmp_request = (struct icmp *) icmp_request_buff;
    icmp_request->icmp_type = ICMP_ECHO;
    icmp_request->icmp_code = htons(0);
    icmp_request->icmp_id = htons(pid);    
    
    int ttl, sequence = 0, replied_packets_num;
    bool stop = 0;
    struct in_addr replied_ip;
    
    for (ttl = 1; ttl <= TTL_LIMIT; ++ttl)
    {
        replied_packets_num = 0;

        if (setsockopt(socket_id, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
        {
            fprintf(stderr, "setsockopt() error: %s\n", strerror(errno));
            return -1;
        }
        icmp_request->icmp_seq = htons(++sequence);
        
        icmp_request->icmp_cksum = 0;
        icmp_request->icmp_cksum = in_cksum((uint16_t*) icmp_request, ICMP_HEADER_LEN);
        
        if (sendto(socket_id, icmp_request_buff, ICMP_HEADER_LEN, 0, (struct sockaddr*)&remote_addr, sizeof(remote_addr)) != ICMP_HEADER_LEN)
        {
            fprintf(stderr, "sendto() error: %s\n", strerror(errno));
            return -1;
        }
    
        gettimeofday(&begin, NULL);
    
        while(replied_packets_num < 1)
        {
            int recv_val = recvfrom(socket_id, reply_buff, BUFFER_SIZE, 0, 0, 0);
            if(recv_val < 0 && errno != EAGAIN)
            {
                fprintf(stderr, "recvfrom() error: %s\n", strerror(errno));
                return -1;
            }
            
            gettimeofday(&current, NULL);
            
            if (recv_val < 0)
            {
                if (time_diff(begin, current) > TIMEOUT)
                    break;
                continue;
            }
            
            struct ip *reply = (struct ip *) reply_buff;
            
            if (reply->ip_p != IPPROTO_ICMP)
                continue; 
            
            struct icmp *icmp_header = (struct icmp *) (reply_buff + reply->ip_hl*4);  
            
            if (icmp_header->icmp_type != ICMP_ECHOREPLY 
               && !(icmp_header->icmp_type == ICMP_TIME_EXCEEDED && icmp_header->icmp_code == ICMP_EXC_TTL))
            {
                continue;
            }
            
            if (icmp_header->icmp_type == ICMP_TIME_EXCEEDED)
            {
                icmp_header = (struct icmp *) (icmp_header->icmp_data + ((struct ip *) (icmp_header->icmp_data))->ip_hl*4);
            }
            
            if (ntohs(icmp_header->icmp_id) != pid || sequence - ntohs(icmp_header->icmp_seq) >= 1)
            {
                continue;
            }
            
            replied_ip = reply->ip_src;
            ++replied_packets_num;
            
            if (icmp_header->icmp_type == ICMP_ECHOREPLY)
                stop = 1;
        }
    
        if(replied_packets_num == 0)
        {
            printf("*\n");
            continue;
        }
        printf("%-16s\n", inet_ntoa(replied_ip));
    
        if (stop == 1)
            break;
    }
    return 0;
}
