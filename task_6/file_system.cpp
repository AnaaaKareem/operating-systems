#include <cstddef>
#include <string>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <cstdio>
using namespace std;

// Define directory processing function
void process_directory(const string& dir_path, int bin_width, int*& bin_count, int& max_bins) {
    // Open directory using directory string and convert to an array of characters for DIR pointer
    DIR* dir = opendir(dir_path.c_str());

    // Check if directory is exists 
    if (dir == NULL) {
        return;
    }

    // Initialize directory entry pointer
    struct dirent* entry;
    
    // Read directory entry by entry
    while ((entry = readdir(dir)) != NULL) {
        
        // Skip current directory and parent directory
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Construct full path of the directory entry
        string full_path = dir_path + "/" + entry->d_name;
        
        // Initialize file status buffer
        struct stat statbuf;

        // Retrieve file metadata into statbuf
        if (lstat(full_path.c_str(), &statbuf) == -1) {
            continue;
        }

        // Check if current path is a directory
        if (S_ISDIR(statbuf.st_mode)) {

            // Call directory processing function for the current directory
            process_directory(full_path, bin_width, bin_count, max_bins);
            
        // Check if current path is a file
        } else if (S_ISREG(statbuf.st_mode)) {

            // Initialize file size variable
            int file_size = statbuf.st_size;
            
            // Initialize bin index based on file size and bin width
            int index = file_size / bin_width;

            // Check if bin index is within the range of bin_count
            if (index >= max_bins) {

                // Initialize new maximum number of bins
                int new_max = index + 1000;

                // Initialize temporary array with new maximum number of bins
                int* tmp = new int[new_max];
                
                // Initialize temporary array with zeros
                for (int i = 0; i < new_max; i++) {
                    tmp[i] = 0;
                }
                
                // Copy bin counts from bin_count to tmp
                for (int i = 0; i < max_bins; i++) {
                    tmp[i] = bin_count[i];
                }
                
                // Delete bin_count
                delete[] bin_count;

                // Set bin count to NULL
                bin_count = NULL;

                // Update bin count and max bins
                bin_count = tmp;
                max_bins = new_max;
            }

            // Increment bin count
            bin_count[index]++;
        }
    }
    
    // Close directory
    closedir(dir);
}

int main(int argc, char* argv[]) {

    // Check if the number of arguments is correct
    if (argc != 3) {
        printf("Usage: %s <directory_path> <bin_width>\n", argv[0]);
        return 1;
    }

    // Initialize path string and bin width
    string path = argv[1];
    int bin_width = stoi(argv[2]);

    // Check if bin width is positive
    if (bin_width <= 0) {
        printf("Error: bin width must be positive\n");
        return 1;
    }

    // Initialize maximum number of bins
    int max_bins = 1000;

    // Initialize bin count array
    int* bin_count = new int[max_bins];

    // Initialize bin count array with zeros
    for (int i = 0; i < max_bins; i++) {
        bin_count[i] = 0;
    }

    // Process directory
    process_directory(path, bin_width, bin_count, max_bins);

    // Initialize found files variable
    bool found_files = false;

    // Iterate through bin count array
    for (int i = 0; i < max_bins; i++) {
        
        // Check if bin count is greater than 0
        if (bin_count[i] > 0) {
            
            // Set found files to true
            found_files = true;
            
            // Calculate the byte range bounds for the current bin
            int range_start = i * bin_width;
            int range_end = range_start + bin_width - 1;
            
            // Print range start and end
            printf("[%d - %d bytes]: ", range_start, range_end);

            // Print hash marks based on bin count
            for (int j = 0; j < bin_count[i]; j++) {
                printf("#");
            }
            
            // Print bin count
            printf(" %d\n", bin_count[i]);
        }
    }

    // Check if no files were found
    if (!found_files) {
        printf("No accessible files found.\n");
    }

    // Delete bin count array
    delete[] bin_count;
  
    return 0;
}