import sys
import time
from socket import *

#command line
if len(sys.argv) != 3:
    print("Usage: python UDPPingerClient.py <server_host> <server_port>")
    sys.exit(1)

#server info from command line
server_host = sys.argv[1]
server_port = int(sys.argv[2])

#UDP socket and timeout
client_sock = socket(AF_INET, SOCK_DGRAM)
client_sock.settimeout(1)

for seq_num in range(1, 11):
    ping_tstamp = time.time()
    ping_Mes = f"PING {seq_num} {time.asctime(time.localtime(ping_tstamp))}"
    try:
        #timer start
        timer_s = time.perf_counter()

        #packet to server
        client_sock.sendto(ping_Mes.encode(), (server_host, server_port))

        #recieve response
        mod_Mes, server_address = client_sock.recvfrom(1024)

        #timer end
        timer_e = time.perf_counter()



        #RTT
        rtt = (timer_e - timer_s)

        print(f"Reply from {server_address[0]}: {mod_Mes.decode()}")
        print(f"RTT: {rtt}")
    except timeout:
        print("Request timed out")

client_sock.close()
