__author__ = 'Andrey Lazarevich'
import socket
import sys

def trace(address):
    port = 33500
    ttl = 1
    icmp_proto = socket.getprotobyname('icmp')
    upd_proto = socket.getprotobyname('udp')
    while ttl < 127:
        ssocket = socket.socket(type=socket.SOCK_DGRAM, proto=upd_proto)
        rsocket = socket.socket(type=socket.SOCK_RAW, proto=icmp_proto)
        ssocket.setsockopt(socket.SOL_IP, socket.IP_TTL, ttl)
        rsocket.bind(('', port))
        ssocket.sendto('', (address, port))

        last_addr = None
        try:
            rsocket.settimeout(20)
            _, last_addr = rsocket.recvfrom(256)
            last_addr = last_addr[0]
            print last_addr
        except socket.timeout:
            print "* * *"
        except socket.error:
            print "* * *"
        finally:
            ssocket.close()
            rsocket.close()

        if last_addr == address and last_addr is not None:
            break
        else:
            ttl += 1


if __name__ == '__main__':
    try:
        ip_addr = sys.argv[1]
        trace(ip_addr)
    except IndexError:
        print "You didn't print ip address"
        exit()