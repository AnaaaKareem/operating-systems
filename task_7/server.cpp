#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
using namespace std;

#define PORT 8080
#define BUFFER_SIZE 1024

// Initialize receive messages function with socket descriptor
void* receive_messages(void* socket_desc) {
    // cast socket descriptor to int
    int sock = *(int*)socket_desc;
    // Initialize buffer to store received messages
    char buffer[BUFFER_SIZE];
    // Initialize bytes received to 0
    int bytes_received;

    // Loop to receive messages
    while (true) {
        // Clear message buffer
        memset(buffer, 0, BUFFER_SIZE);
        // Receive messages
        bytes_received = recv(sock, buffer, BUFFER_SIZE, 0);
        
        // Check if connection is closed
        if (bytes_received <= 0) {
            // Print message
            printf("\nConnection with the client has been lost.\n");
            // Close the socket
            exit(0);
        }
        
        // Print the received message
        printf("Client: %s\nYou: ", buffer);
        // Flush the output
        fflush(stdout);
    }
    // Return NULL
    return NULL;
}

int main() {
    // Initialize server file descriptor
    int server_fd, new_socket;
    // Initialize address structure
    struct sockaddr_in address;
    // Initialize options
    int opt = 1;
    // Initialize address length
    int addrlen = sizeof(address);
    // Initialize thread ID
    pthread_t thread_id;

    // Create socket specifying IPv4, TCP, and 0 for default protocol
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    // Set socket options to allow reuse of the address
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Set address family to IPv4
    address.sin_family = AF_INET;
    // Set address to any IP address
    address.sin_addr.s_addr = INADDR_ANY;
    // Set port number
    address.sin_port = htons(PORT);

    // Bind the socket to the address
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    // Listen for incoming connections
    listen(server_fd, 3);

    // Print message
    printf("Waiting for connection...\n");
    // Accept incoming connection
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    // Print message
    printf("Connected! Start chatting.\n");

    // Create a thread to handle incoming messages
    if (pthread_create(&thread_id, NULL, receive_messages, (void*)&new_socket) < 0) {
        // Print error message
        perror("could not create thread");
        // Return error code
        return 1;
    }

    // Main thread handles sending messages
    char msg[BUFFER_SIZE];
    while (true) {
        // Print message
        printf("You: ");
        // Get message from user
        fgets(msg, BUFFER_SIZE, stdin);
        // Remove newline character from fgets
        msg[strcspn(msg, "\n")] = 0;
        
        // Send the message
        if (send(new_socket, msg, strlen(msg), 0) < 0) {
            // Break if sending fails
            break;
        }
    }

    // Close the sockets
    close(new_socket);
    close(server_fd);

    return 0;
}