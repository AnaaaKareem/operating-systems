#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
using namespace std;

// Assigning page table and page fault counter
map<int, uint8_t> pageTable;
int page_faults = 0;

// Initialize page fault function
void managePageFaults(int process, int num_frames) {
    // Shift the frames in page table to the right
    for (auto& frame : pageTable) {
        frame.second = frame.second >> 1;
    }

    // Check if process is in the page table
    if (pageTable.find(process) != pageTable.end()) {
        // Assign process with the most recent access
        pageTable[process] = pageTable[process] | 128;
    }

    else {
        // Increment page fault counter
        page_faults++;
        
        // Check if page table is full
        if (pageTable.size() >= num_frames) {

            // Assign least recently used process
            int least_used_process = -1;

            // Assign minimum age
            uint8_t min_age = 255;

            // Search for the least recently used process
            for (auto const& frame : pageTable) {
                // Check if the current process is the least recently used
                if (frame.second < min_age) {
                    // Assign minimum age
                    min_age = frame.second;
                    // Assign least recently used process
                    least_used_process = frame.first;
                }
            }

            // Remove the least recently used process
            pageTable.erase(least_used_process);
        }
        // Assign new process with the most recent access
        pageTable[process] = 128;
    }
}

int main(int argc, char* argv[]) {

    // Check if file exists
    if (argc != 3) {
        printf("Usage: %s <filename> <number of frames>\n", argv[0]);
        return 1;
    }

    // Open file
    FILE* file = fopen(argv[1], "r");
    
    // Check if file exists
    if (!file) {
        printf("Error: File does not exist\n");
        return 1;
    }

    // Check if number of frames is a positive integer
    if (atoi(argv[2]) <= 0 || !isdigit(atoi(argv[2]))) {
        printf("Error: Number of frames must be a positive integer\n");
        return 1;
    }

    // Assign number of frames
    int num_frames = atoi(argv[2]);

    // Initialize process ID variable
    int process;

    // Iterate through the file
    while(fscanf(file, "%d", &process) == 1) {
        // Manage page faults
        managePageFaults(process, num_frames);
    }

    // Close file
    fclose(file);
    
    // Print page faults
    printf("Page Faults: %d\n", page_faults);
    return 0;
}