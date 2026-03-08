#include <iostream>
#include <stdio.h>

// Initialize deadlock detector function
void detectDeadlock(int processes, int resource_types, int* existing, int** allocation, int** requests) {
    
    // Initialize and calculate availablility vector
    int* available = new int[resource_types];
    for (int i = 0; i < resource_types; i++) {
        // Start total allocations counter for a resource type
        int total_allocations = 0;
        for (int j = 0; j < processes; j++) {
            // Count the processes used for each resource type
            total_allocations += allocation[j][i];
        }
        // Update available resources
        available[i] = existing[i] - total_allocations;
    }

    // Initialize completed process array
    bool* completed = new bool[processes];
    for (int i = 0; i < processes; i++) {
        completed[i] = false;
    }

    // Deadlock detection
    bool progress = true;
    while (progress) {
        progress = false;
        // Iterate through all processes
        for (int i = 0; i < processes; i++) {
            // Check if proccess is not completed
            if (!completed[i]) {
                // Check if request is smaller than available resources
                bool can_satisfy = true;
                // Iterate through all resource types
                for (int j = 0; j < resource_types; j++) {
                    // Check if the current request is bigger than available resources
                    if (requests[i][j] > available[j]) {
                        // Set satisfy to false and break the loop
                        can_satisfy = false;
                        break;
                    }
                }

                // If the request can be satisfied
                if (can_satisfy) {
                    // Process finishes and returns resources to Available pool
                    for (int j = 0; j < resource_types; j++) {
                        available[j] += allocation[i][j];
                    }
                    completed[i] = true;
                    progress = true;
                }
            }
        }
    }

    // Initialize deadlock process vector and counter
    int* deadlocked_processes = new int[processes];
    int deadlock_counter = 0;

    // Iterate through all incomplete process
    for (int i = 0; i < processes; i++) {
        if (!completed[i]) {
            // Add process id to the vector
            deadlocked_processes[deadlock_counter] = i;
            deadlock_counter++;
        }
    }

    // Print all deadlock processes
    if (deadlock_counter > 0) {
        printf("Deadlocked processes: ");
        // Iterate and print all deadlock processes
        for (int i = 0; i < deadlock_counter; i++) {
            printf("%d ", deadlocked_processes[i]);
        }
        printf("\n");
    } else {
        // Print if no deadlock existed
        printf("No deadlock detected.\n");
    }

    // Clean up function variables from memory
    delete[] available;
    delete[] completed;
    delete[] deadlocked_processes;
}

int main(int argc, char* argv[]) {

    // Check command line arguments
    if (argc < 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }

    // Create file pointer
    FILE* file;

    // Open file using pointer
    file = fopen(argv[1], "r");

    // Check if file exists
    if (file == NULL) {
        printf("File does not exist\n");
        return 1;
    }

    // Initialize proccess and resource types
    int processes, resource_types; 

    fscanf(file, "%d %d", &processes, &resource_types);

    // Setup existing array
    int* existing = new int[resource_types];
    for (int i = 0; i < resource_types; i++) {
        fscanf(file, "%d", &existing[i]);
    }

    // Setup allocation matrix 
    int** allocation = new int*[processes];
    for (int i = 0; i < processes; i++) {
        allocation[i] = new int[resource_types];
        for (int j = 0; j < resource_types; j++) {
            fscanf(file, "%d", &allocation[i][j]);
        }
    }
        
    // Setup request matrix
    int** requests = new int*[processes];
    for (int i = 0; i < processes; i++) {
        requests[i] = new int[resource_types];
        for (int j = 0; j < resource_types; j++) {
            fscanf(file, "%d", &requests[i][j]);
        }
    }

    // Close file pointer
    fclose(file);

    // Start Deadlock Detector
    detectDeadlock(processes, resource_types, existing, allocation, requests);

    // Free variable and matricies memory
    delete[] existing;
    for (int i = 0; i < processes; i++) {
        delete[] allocation[i];
        delete[] requests[i];
    }
    delete[] allocation;
    delete[] requests;

    return 0;
}