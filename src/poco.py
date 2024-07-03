import socket

def start_server():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(('localhost', 3001))
    server_socket.listen(1)

    print("Server listening on port 3001...")

    conn, addr = server_socket.accept()
    print(f"Connection from {addr}")

    data = conn.recv(1024)
    if data:
        print(f"Received message: {data.decode('utf-8')}")

    conn.close()
    server_socket.close()

if __name__ == "__main__":
    start_server()