#!/usr/bin/env python3

import socket
import sys


def nth_addr(dest_addr, ttl):
    port = 33434
    icmp = socket.getprotobyname('icmp')
    udp = socket.getprotobyname('udp')

    in_socket = socket.socket(socket.AF_INET, socket.SOCK_RAW, icmp)

    out_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, udp)
    out_socket.setsockopt(socket.SOL_IP, socket.IP_TTL, ttl)

    in_socket.bind((bytes("", 'UTF-8'), port))
    in_socket.settimeout(60)

    out_socket.sendto(bytes("", 'UTF-8'), (dest_addr, port))

    try:
        _, result = in_socket.recvfrom(256)
        return result[0]
    except socket.error:
        return None
    finally:
        out_socket.close()
        in_socket.close()


def tracert(addr):
    max_ttl = 64

    for ttl in range(1, max_ttl):
        current_addr = nth_addr(addr, ttl)

        if current_addr is None:
            print("* * *")
        else:
            print(current_addr)

        if current_addr == addr:
            return

    print("Destination is unavailable")


if __name__ == "__main__":
    tracert(sys.argv[1])