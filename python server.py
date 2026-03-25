import socket

HOST = "0.0.0.0"
PORT = 5000

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((HOST, PORT))
server.listen()

print("Server started")

while True:
    conn, addr = server.accept()
    print("Connection from:", addr)

    data = conn.recv(1024)

    if data:
        msg = data.decode().strip()
        print("Received:", msg)
    else:
        print("No data received")

    conn.close()