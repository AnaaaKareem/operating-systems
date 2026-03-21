#include <iostream>
#include <algorithm>
#include <cstdio>
#include <queue>
using namespace std;

// Initialize process structure with id, arrival time, burst time, and wait time
struct process {
    int id;
    int arrival;
    int burst;
    int wait;
};

// Initialize First Come First Serve Scheduling Algorithm
float FCFS(process* processes, int num_processes) {

    // Check if there are any processes
    if (num_processes == 0) {
        return 0;
    }

    // Initialize time related variables
    int currentTime = 0;
    int totalWaitTime = 0;

    // Iterate through all processes
    for (int i = 0; i < num_processes; i++) {
        // Check if current time is less than the arrival time
        if (currentTime < processes[i].arrival) {
            // Set current time to arrival time
            currentTime = processes[i].arrival;
        }
        // Calculate wait time
        processes[i].wait = currentTime - processes[i].arrival;
        // Add wait time to total wait time
        totalWaitTime += processes[i].wait;
        // Add burst time to current time
        currentTime += processes[i].burst;
    }

    // Calculate average wait time
    float averageWaitTime = (float) totalWaitTime / num_processes;

    // Return average wait time
    return averageWaitTime;
}

// Initialize Non-Preemptive Shortest Job First Scheduling Algorithm
float NPSJF(process* processes, int num_processes) {

    // Check if there are any processes
    if (num_processes == 0) {
        return 0;
    }

    // Initialize time related/tracking variables
    int currentTime = 0;
    int totalWaitTime = 0;
    int completedCount = 0;

    // Set trackness of completed processes to false
    int* isCompleted = new int[num_processes];
    for (int i = 0; i < num_processes; i++) {
        isCompleted[i] = 0;
    }

    // Loop through all processes until they are completed
    while (completedCount < num_processes) {
        // Initialize id and minimum burst variables
        int idx = -1;
        int minBurst = 1000000000;

        // Iterate over all processes
        for (int i = 0; i < num_processes; i++) {
            // Check if a process has arrived and is not completed
            if (processes[i].arrival <= currentTime && !isCompleted[i]) {
                // Check if current process is the shortest burst time
                if (processes[i].burst < minBurst) {
                    // Update shortest burst time and process index
                    minBurst = processes[i].burst;
                    idx = i;
                }
                // If burst times are equal, pick the one that arrived first
                else if (processes[i].burst == minBurst) {
                    // Update process index if current process arrived first
                    if (processes[i].arrival < processes[idx].arrival) {
                        idx = i;
                    }
                }
            }
        }

        // Check if a process was found
        if (idx != -1) {
            // Update wait time for current process and add to total wait time
            processes[idx].wait = currentTime - processes[idx].arrival;
            totalWaitTime += processes[idx].wait;

            // Update current time and mark process as completed
            currentTime += processes[idx].burst;
            isCompleted[idx] = 1;
            completedCount++;
        } else {
            // Find next arrival time
            int nextArrival = 1000000000;
            for (int i = 0; i < num_processes; i++) {
                // Check if process is not completed and has arrived
                if (!isCompleted[i] && processes[i].arrival < nextArrival) {
                    nextArrival = processes[i].arrival;
                }
            }
            // Update current time
            currentTime = nextArrival;
        }
    }

    // Free memory
    delete[] isCompleted;

    // Calculate average wait time
    float averageWaitTime = (float) totalWaitTime / num_processes;

    // Return average wait time
    return averageWaitTime;
}

// Initialize Preemptive Shortest Job First Scheduling Algorithm
float PSJF(process* processes, int num_processes) {

    // Check if there are any processes
    if (num_processes == 0) {
        return 0;
    }

    // Initialize time related/tracking variables
    int currentTime = 0;
    int totalWaitTime = 0;
    int completedCount = 0;

    // Set trackness of completed processes to false
    int* remainingBurst = new int[num_processes];
    for (int i = 0; i < num_processes; i++) {
        remainingBurst[i] = processes[i].burst;
    }

    // Loop throughall processes until they are completed
    while (completedCount < num_processes) {
        // Initialize id and minimum burst variables
        int idx = -1;
        int minRemaining = 1000000000;

        // Iterate over all processes
        for (int i = 0; i < num_processes; i++) {
            // Check if a process has arrived and is not completed
            if (processes[i].arrival <= currentTime && remainingBurst[i] > 0) {
                // Check if current process is the shortest burst time
                if (remainingBurst[i] < minRemaining) {
                    // Update shortest burst time and process index
                    minRemaining = remainingBurst[i];
                    idx = i;
                }
                // If burst times are equal, pick the one that arrived first
                else if (remainingBurst[i] == minRemaining) {
                    // Update process index if current process arrived first
                    if (idx != -1 && processes[i].arrival < processes[idx].arrival) {
                        idx = i;
                    }
                }
            }
        }

        // Check if a process was found
        if (idx != -1) {
            // Update wait time for current process and add to total wait time
            remainingBurst[idx]--;
            currentTime++;

            // Check if the process just finished
            if (remainingBurst[idx] == 0) {
                completedCount++;

                // Wait Time = Finish Time - Arrival Time - Original Burst Time
                processes[idx].wait =
                    currentTime - processes[idx].arrival - processes[idx].burst;
                totalWaitTime += processes[idx].wait;
            }

        } else {
            // Find next arrival time
            int nextArrival = 1000000000;
            for (int i = 0; i < num_processes; i++) {
                // Check if process is not completed and has arrived
                if (remainingBurst[i] > 0 && processes[i].arrival > currentTime) {
                    if (processes[i].arrival < nextArrival) {
                        nextArrival = processes[i].arrival;
                    }
                }
            }
            // Update current time
            currentTime = nextArrival;
        }
    }

    // Free memory
    delete[] remainingBurst;

    // Calculate average wait time
    float averageWaitTime = (float) totalWaitTime / num_processes;

    // Return average wait time
    return averageWaitTime;
}

// Initialize Round Robin Scheduling Algorithm
float RR(process* processes, int num_processes, int quantum) {

    // Check if there are any processes
    if (num_processes <= 0) {
        return 0;
    }

    // Initialize time related/tracking variables
    int currentTime = 0;
    int totalWaitTime = 0;
    int completed_processes = 0;
    int next_arrival_index = 0;

    // Track remaining burst time for each process
    int* remainingBurst = new int[num_processes];
    for (int i = 0; i < num_processes; i++) {
        remainingBurst[i] = processes[i].burst;
    }

    // Initialize ready queue
    queue < int > readyQueue;

    // Loop through all processes until they are completed
    while (completed_processes < num_processes) {
        // If the queue is empty and there are processes that have not arrived yet
        if (readyQueue.empty() && next_arrival_index < num_processes) {
            // If the current time is less than the next arrival time
            if (currentTime < processes[next_arrival_index].arrival) {
                // Update current time to the next arrival time
                currentTime = processes[next_arrival_index].arrival;
            }
        }

        // Add all processes that have arrived by the current time to the queue
        while (next_arrival_index < num_processes &&
            processes[next_arrival_index].arrival <= currentTime) {
            // Add the process to the ready queue
            readyQueue.push(next_arrival_index);
            // Increment the next arrival index
            next_arrival_index++;
        }

        // If the ready queue is not empty
        if (!readyQueue.empty()) {
            // Get the next process in line
            int idx = readyQueue.front();
            readyQueue.pop();

            // Determine execution time (either the full quantum or what's left)
            int execute_time = min(remainingBurst[idx], quantum);

            // Move time forward
            currentTime += execute_time;
            remainingBurst[idx] -= execute_time;

            // Check for arrivals while the current process is running
            while (next_arrival_index < num_processes &&
                processes[next_arrival_index].arrival <= currentTime) {
                // Add the process to the ready queue
                readyQueue.push(next_arrival_index);
                // Increment the next arrival index
                next_arrival_index++;
            }

            // If the process has not finished, put it back at the end of the queue
            if (remainingBurst[idx] > 0) {
                readyQueue.push(idx);
            } else {
                // Process is finished
                completed_processes++;
                // Calculate wait time
                int waitTime =
                    currentTime - processes[idx].arrival - processes[idx].burst;
                // Update wait time
                processes[idx].wait = waitTime;
                // Add to total wait time
                totalWaitTime += waitTime;
            }
        }
    }

    // Free memory
    delete[] remainingBurst;

    // Calculate average wait time
    float averageWaitTime = (float) totalWaitTime / num_processes;

    // Return average wait time
    return averageWaitTime;
}

// Compare function for sorting processes by arrival time
bool compareArrival(const process & a, const process & b) {
    if (a.arrival != b.arrival) {
        return a.arrival < b.arrival;
    }
    return a.id < b.id;
}

int main(int argc, char * argv[]) {
    // Check command line arguments
    if (argc < 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }

    // Create file pointer
    FILE * file;

    // Open file using file pointer
    file = fopen(argv[1], "r");

    // Check if file exists
    if (file == NULL) {
        printf("File does not exist\n");
        return 1;
    }

    // Initialize process array
    process * processes = new process[100];

    // Initialize file reading variables
    int id, arrival, burst;

    // Initialize process count
    int capacity = 100;
    int process_count = 0;

    // Read file line by line (using == 3 to ensure valid parsing)
    while (fscanf(file, "%d %d %d", & id, & arrival, & burst) == 3) {
        // Add process to array explicitly
        processes[process_count].id = id;
        processes[process_count].arrival = arrival;
        processes[process_count].burst = burst;
        processes[process_count].wait = 0;

        // Update process count
        process_count++;

        // Check if array needs to be resized
        if (capacity == process_count) {
            // Resize array
            process* tmp = new process[capacity * 2];
            // Copy processes to new array
            for (int i = 0; i < capacity; i++) {
                tmp[i] = processes[i];
            }
            // Free old array
            delete[] processes;
            // Update process pointer
            processes = tmp;
            capacity *= 2;
        }
    }

    // Close file
    fclose(file);

    // Prevent divide-by-zero or empty issues
    if (process_count == 0) {
        printf("No processes found or file is empty\n");
        delete[] processes;
        return 0;
    }

    // Sort processes based on arrival time
    sort(processes, processes + process_count, compareArrival);

    // Make local copies for each scheduling algorithm to prevent
    // shared memory mutations (like wait times) overwriting each other
    process* fcfs = new process[process_count];
    process* npsjf = new process[process_count];
    process* psjf = new process[process_count];
    process* rr = new process[process_count];

    for (int i = 0; i < process_count; i++) {
        fcfs[i] = processes[i];
        npsjf[i] = processes[i];
        psjf[i] = processes[i];
        rr[i] = processes[i];
    }

    // Initialize average wait time variables
    float avg_fcfs = FCFS(fcfs, process_count);
    float avg_NPsjf = NPSJF(npsjf, process_count);
    float avg_psjf = PSJF(psjf, process_count);
    float avg_rr = RR(rr, process_count, 1);

    // Print to console
    cout << "FCFS " << avg_fcfs << endl;
    cout << "NPSJF " << avg_NPsjf << endl;
    cout << "PSJF " << avg_psjf << endl;
    cout << "RR " << avg_rr << endl;

    // Free memory
    delete[] fcfs;
    delete[] npsjf;
    delete[] psjf;
    delete[] rr;
    delete[] processes;

    return 0;
}