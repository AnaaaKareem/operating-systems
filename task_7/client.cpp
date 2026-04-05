#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
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

    while (true) {
        // Clear message buffer
        memset(buffer, 0, BUFFER_SIZE);
        // Receive messages
        bytes_received = recv(sock, buffer, BUFFER_SIZE, 0);
        
        // Check if connection is closed
        if (bytes_received <= 0) {
            // Print message
            printf("\nConnection with the server has been lost.\n");
            // Close the socket
            exit(0);
        }
        // Print the received message
        printf("\rServer: %s\nYou: ", buffer);
        // Flush the output
        fflush(stdout);
    }
    // Return NULL
    return NULL;
}

int main() {
    // Initialize socket descriptor
    int sock = 0;
    // Initialize server address structure
    struct sockaddr_in serv_addr;
    // Initialize thread ID
    pthread_t thread_id;

    // Create socket specifying IPv4, TCP, and 0 for default protocol
    sock = socket(AF_INET, SOCK_STREAM, 0);
    // Set address family to IPv4
    serv_addr.sin_family = AF_INET;
    // Set port number
    serv_addr.sin_port = htons(PORT);
    // Set server address to localhost
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        // Print error message
        printf("Connection Failed\n");
        // Return -1
        return -1;
    }

    // Print message
    printf("Connected! Start chatting.\n");

    // Create a thread to handle incoming messages
    pthread_create(&thread_id, NULL, receive_messages, (void*)&sock);

    // Initialize buffer to store messages
    char msg[BUFFER_SIZE];
    while (true) {
        // Print message
        printf("You: ");
        // Get message from user
        fgets(msg, BUFFER_SIZE, stdin);
        // Remove newline character from fgets
        msg[strcspn(msg, "\n")] = 0;

        // Send the message
        if (send(sock, msg, strlen(msg), 0) < 0) {
            // Break if sending fails
            break;
        }
    }

    // Close the socket
    close(sock);

    return 0;
}