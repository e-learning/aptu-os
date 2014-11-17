#include <stdio.h>
#include <string.h>

#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define MAX_HOPS 127
#define PORT 33123
#define PACKET_SIZE 64


int main(int argc, char** argv)
{
	int return_code = 0;
	int recv_socket = -1;
	int send_socket = -1;
	int ttl;
	int rc;
	char data[PACKET_SIZE];
	struct sockaddr_in local_addr;
	struct sockaddr_in target_addr;
	struct sockaddr_in responce_addr;
	char ipaddr[INET_ADDRSTRLEN];
	socklen_t addrlen;
	struct timeval timeout;

	if (argc != 2)
	{
		printf("ip address is needed\n");
		return_code = -1;
		goto finally;
	}

	recv_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (recv_socket < 0)
	{
		printf("ICMP socket error\n");
		return_code = -1;
		goto finally;
	}

	send_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (send_socket < 0)
	{
		printf("UDP socket error\n");
		return_code = -1;
		goto finally;
	}

	memset(&local_addr, 0x00, sizeof(local_addr));
	local_addr.sin_family = AF_INET;
	//local_addr.sin_addr.s_addr = INADDR_LOOPBACK;
	local_addr.sin_port = htons(PORT);

	rc = bind(recv_socket, (struct sockaddr *)&local_addr, sizeof(local_addr));
	if (rc < 0)
	{
		printf("bind error\n");
		return_code = -1;
		goto finally;
	}

	memset(&timeout, 0x00, sizeof(timeout));
	timeout.tv_sec = 5;
	rc = setsockopt(recv_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
	if (rc < 0)
	{
		printf("set timeout error\n");
		return_code = -1;
		goto finally;
	}

	memset(&target_addr, 0x00, sizeof(target_addr));
	target_addr.sin_family = AF_INET;
	inet_aton(argv[1], &target_addr.sin_addr);
	target_addr.sin_port = htons(PORT);

	for (ttl = 1; ttl < MAX_HOPS; ttl++)
	{
		setsockopt(send_socket, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
		rc = sendto(send_socket, NULL, 0, 0,
			(struct sockaddr *)&target_addr, sizeof(target_addr));
		if (rc < 0)
		{
			printf("sendto error\n");
			return_code = -1;
			goto finally;
		}
		addrlen = sizeof(responce_addr);
		rc = recvfrom(recv_socket, data, PACKET_SIZE, 0,
			(struct sockaddr *)&responce_addr, &addrlen);
		if (rc < 0)
		{
			printf("recvfrom error\n");
			continue;
		}
		inet_ntop(responce_addr.sin_family, &(responce_addr.sin_addr),
			ipaddr, sizeof(ipaddr));
		printf("%d: %s\n", ttl, ipaddr);
		if (strcmp(ipaddr, argv[1]) == 0)
		{
			break;
		}
	}


	finally:
	if (recv_socket >= 0) close(recv_socket);
	if (send_socket >= 0) close(send_socket);
	return return_code;
}