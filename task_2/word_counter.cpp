#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <pthread.h>
#include <ctype.h>
using namespace std;

// Global words container
vector<string> words;

// Each thread has its own map
vector<map<string, int>> word_count;

// Structure to pass to each thread
struct ThreadData {
    int thread_id;
    int start;
    int end;
};

// Initialize function to remove special characters
void remove_special_characters(char* word) {
    int i = 0;
    while (word[i] != '\0') {
        //check if current character is not an alphabet
        if (!isalpha(word[i])) {
            word[i] = '\0';
        }
        i++;
    }
}

// Initialize word count thread function
void* word_counter(void* arg) {
    // Get thread data
    ThreadData* data = (ThreadData*)arg;

    // Count words
    for (int i = data->start; i < data->end; i++) {
        // Increment word count based on a thread id
        word_count[data->thread_id][words[i]]++;
    }

    // Exit thread
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {

    // Check if filename and the number of threads are provided
    if (argc != 3) {
        printf("Usage: %s <filename> <num_threads>\n", argv[0]);
        return 1;
    }

    // Set number of threads defined by the user
    int num_threads = atoi(argv[2]);

    // Open file
    FILE* file = fopen(argv[1], "r");

    // Check if file exists
    if (!file) {
        printf("Error: File does not exist\n");
        return 1;
    }
    
    // Initialize word character array
    char* word = new char[100];
    
    // Read all words
    while (fscanf(file, "%s", word) != EOF) {
        // Remove special characters
        remove_special_characters(word);
        // Add word to words vector
        words.push_back(word);
    }

    // Delete word character array
    delete[] word;
    word = NULL;

    // Close file
    fclose(file);

    // Get total words
    int total_words = words.size();

    // Resize map based on number of threads
    word_count.resize(num_threads);
    
    // Initialize threads and thread data
    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];

    // Define segment size based on the total words and number of threads
    int segment_size = total_words / num_threads;

    // Create threads
    for (int i = 0; i < num_threads; i++) {
        // Set thread id
        thread_data[i].thread_id = i;
        // Set thread start
        thread_data[i].start = i * segment_size;

        // Set thread end
        if (i == num_threads - 1) {
            thread_data[i].end = total_words;
        } else {
            thread_data[i].end = (i + 1) * segment_size;
        }

        // Create thread
        pthread_create(&threads[i], NULL, word_counter, &thread_data[i]);
    }

    // Wait for threads
    for (int i = 0; i < num_threads; i++) {
        // Wait for thread to finish
        pthread_join(threads[i], NULL);
    }

    // Combine results
    map<string, int> final_count;

    // Combine word count from each thread
    for (int i = 0; i < num_threads; i++) {
        for (auto it = word_count[i].begin(); it != word_count[i].end(); ++it) {
            // Add word count to final count
            final_count[it->first] += it->second;
        }
    }

    // Print results
    for (auto it = final_count.begin(); it != final_count.end(); ++it) {
        // Print word and count
        printf("%s : %d\n", it->first.c_str(), it->second);
    }

    return 0;
}