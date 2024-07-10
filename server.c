#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    int new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Setting server_fd to -1 by standard to not use value previously allocated at the same memory location.
    int server_fd = -1;

    // Checking for server File Descriptor errors.
    // Socket() returns a -1 for failed socket setup.
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Set Socket options with SOL_SOCKET which is socket layer,
    // used as the "level" parameter in setsockopt() to indicate that we're setting a socket-level option,
    // as opposed to a protocol-specific option.
    // The value of opt (1 in this case) is used to enable the SO_REUSEADDR option.
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    };

    // Configuration for the server to listen to any available interfaces on the specified port (80).
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the specified address and port.
    if (bind(server_fd, (struct sockaddr *)&address, addrlen) < 0)
    {
        perror("Socket binding failed.");
        exit(EXIT_FAILURE);
    }

    // This marks the socket as a passive socket that will be used to accept incoming connection requests.
    if (listen(server_fd, 5) < 0)
    {
        perror("Listening fails.");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    // Waits for a client to connect to the server socket, accept() creates a new socket to handle communication with client.
    // (struct sockaddr *)&address argument is a pointer to a structure that holds information about the client’s address.
    // If the connection is successful, accept() returns a new file descriptor (socket) representing the connection.
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("Error accepting client connection.");
        exit(EXIT_FAILURE);
    }

    printf("Connection Accepted\n");

    // // Read data from the client socket, print it, and close the server socket.
    ssize_t valread;
    while ((valread = read(new_socket, buffer, BUFFER_SIZE)) > 0)
    {
        printf("Client: %s", buffer);
        // This step ensures that any residual data from the previous read operation is removed.
        memset(buffer, 0, sizeof(buffer));
    }

    close(server_fd);
    return 0;
}