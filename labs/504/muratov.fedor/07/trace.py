#!/usr/bin/env python3
import socket
import sys
import os


def ping(address):
    return os.system("ping -w 1 " + address)


def trace(address):
    port = 12345
    ttl = 1
    max_ttl = 256
    icmp = socket.getprotobyname('icmp')
    upd = socket.getprotobyname('udp')
    traceWay = []
    wrongs = 0
    print("hop ")
    while ttl < max_ttl:
        print("{0}".format(ttl), end=" ")
        sys.stdout.flush()
        senderSocket = socket.socket(type=socket.SOCK_DGRAM, proto=upd)
        recieverSocket = socket.socket(type=socket.SOCK_RAW, proto=icmp)
        senderSocket.setsockopt(socket.SOL_IP, socket.IP_TTL, ttl)
        recieverSocket.bind((bytes('', encoding='utf-8'), port))
        senderSocket.sendto(bytes('', encoding='utf-8'), (address, port))
        lastAddr = None
        try:
            recieverSocket.settimeout(1.0)
            (data, lastAddr) = recieverSocket.recvfrom(1)
            traceWay.append(lastAddr[0])
        except socket.timeout:
            traceWay.append("hop timeout")
            wrongs += 1
        except socket.error:
            traceWay.append("hop error")
            wrongs += 1
        finally:
            senderSocket.close()
            recieverSocket.close()

        if (lastAddr is not None and lastAddr[0] == address) or wrongs > max_ttl/2:
            break
        else:
            ttl += 1
    print()
    return traceWay


if __name__ == '__main__':
    try:
        ip_addr = sys.argv[1]
        print("ping {0}".format(ip_addr))
        ans = ping(ip_addr)
        if not ans:
            print("trace to {0}".format(ip_addr))
            ans = trace(ip_addr)
            print("trace:")
            for hop in ans:
                print("\t", hop)
        else:
            print("wrong ping, try latter")
    except IndexError:
        print("usage: param:ip_to_trace")