# INET3101 Module 5 Assignment 5

# Problem Statement

In this assignment, I implemented a simple client-server program using C sockets.

- The **server** receives a filename from the client and sends back the file’s text contents.
- The file content must be **255 characters or less**.
- The **client** connects to the server, requests a file, and prints the response.
- The program handles errors like invalid filename, file not found, or file too large.
- The server can shut down gracefully when terminated.

# Describe the Solution

This project contains:
- `server.c`: Creates a socket, binds to a port, listens for connections, accepts a client, reads a filename from the socket, validates it, attempts to open/read the file, checks the content length (max 255 chars), then sends either the file content or an error message back to the client.
- `client.c`: Creates a socket, connects to the server (IP + port), sends the filename, then reads the server response and prints it.

# Pros and Cons of the Solution

### Pros
- Simple and easy to understand.
- Handles common errors properly.
- Works well for small text files.

### Cons
- Only supports small files (≤255 chars).
- Handles one request at a time.
- Very basic protocol with no extra features.
