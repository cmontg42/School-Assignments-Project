
import hashlib
import os
import sys
from socket import*
from urllib.parse import urlparse

CACHE = "cache"
BUFFER = 4096

if not os.path.exists(CACHE):
    os.makedirs(CACHE)

if len(sys.argv) != 2:
    print("Usage: python ProxyServer.py <server_host>")
    sys.exit(1)

proxy_host = sys.argv[1]
proxy_port = 8888

#TCP socket
server_sock = socket(AF_INET, SOCK_STREAM)
server_sock.bind((proxy_host, proxy_port))
server_sock.listen(5)

print(f"Proxy server on {proxy_host}:{proxy_port}")

def handle_client(client_sock):
    try:
        request_sin = client_sock.recv(BUFFER).decode(errors = 'ignore')
        if not request_sin:
            client_sock.close()
            return

        #http first line request
        request_line = request_sin.split('\n')[0]
        parts = request_line.split()

        if len(parts) < 3:
            client_sock.close()
            return

        method, url, http_version = parts

        if method != "GET":
            response = (
                "HTTP/1.0 405 Method Not Allowed\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 22\r\n"
                "\r\n"
                "405 Method Not Allowed"
            )
            client_sock.sendall(response.encode())
            client_sock.close()
            return

        #parse URL
        parsed_URL = urlparse(url)
        if parsed_URL.hostname:
            host = parsed_URL.hostname
            port = parsed_URL.port if parsed_URL.port else 80
            path = parsed_URL.path if parsed_URL.path else "/"
        else:
            host = None
            for line in request_sin.split("\n"):
                if line.lower().startswith('host:'):
                    host = line.split(':', 1)[1].strip()
                    break
            if not host:
                client_sock.close()
                return
            port = 80
            path = parsed_URL.path or "/"

        #build cache filename
        safe_path = path.strip("/").replace("/", "_")
        if not safe_path:
            safe_path = "index.html"
        cache_filename = os.path.join(CACHE, f"{host}_{port}_{safe_path}")

        #check if cached
        if os.path.exists(cache_filename):
            print(f"Cache HIT for {cache_filename}")
            client_sock.setblocking(True)
            with open(cache_filename, "rb") as cache_file:
                cache_data = cache_file.read()

            response_head = (
                "HTTP/1.0 200 OK\r\n"
                f"Content-Length: {len(cache_data)}\r\n"
                "Content-Type: text/html\r\n"
                "Connection: close\r\n\r\n"
            ).encode()
            client_sock.sendall(response_head + cache_data)
            client_sock.close()
            return

        #origin server
        try:
            origin_sock = socket(AF_INET, SOCK_STREAM)
            origin_sock.connect((host, port))
        except Exception as e:
            print(f"Error connecting to {host}:{port} -> {e}")
            response = (
                "HTTP/1.0 502 Bad Gateway\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 15\r\n"
                "\r\n"
                "502 Bad Gateway"
            )
            client_sock.sendall(response.encode())
            client_sock.close()
            return

        #http get request origin
        origin_req =(
            f"GET {path} HTTP/1.0\r\n"
            f"HOST: {host}\r\n"
            f"Connection: close\r\n"
            f"User-Agent: SimpleProxy/1.0\r\n\r\n"
        )
        origin_sock.sendall(origin_req.encode())

        #reponse forward to cache
        with open(cache_filename, "wb") as cache_file:
            while True:
                data = origin_sock.recv(BUFFER)
                if not data:
                    break
                client_sock.sendall(data)
                cache_file.write(data)

        origin_sock.close()
        client_sock.close()

    except Exception as e:
        print(f"Error handling client: {e}")
        client_sock.close()


#main loop
try:
    while True:
        client_socket, client_address = server_sock.accept()
        print(f"Connection from {client_address}")
        handle_client(client_socket)
except KeyboardInterrupt:
    print("\nShutting down proxy server")
    server_sock.close()

