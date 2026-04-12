#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include "crypt.h"

using namespace std;

// Main function entry point to test password hashing and verification
int main() {

    // Seed the random number generator using the current system time
    srand(time(0));

    // Initialize a vector containing a list of test passwords
    vector<string> passwords = {
        "admin123", "Password", "p@SswOrd", "letmein", "tester",
        "superman", "coventry", "secr3t_!", "devops99", "fintechX"
    };

    // Define a structure to store password, salt, and resulting hash together
    struct EncryptedData {
        // Original password string
        string password;
        // Integer salt value used for hashing
        int salt;
        // Resulting cryptographic hash as a 64-bit integer
        long long int hash;
    };

    // Initialize a vector to act as a database for storing encrypted records
    vector<EncryptedData> database;

    // Print header for the hash generation phase
    cout << "--- Generating 10 Encrypted Passwords ---\n";

    // Iterate through each password in the passwords vector using a numerical loop
    for (int i = 0; i < passwords.size(); i++) {
        string pwd = passwords[i];

        // Generate a random 16-bit salt value (0 - 65535)
        int salt = rand() % 65536;

        // Generate the cryptographic hash as a long long int
        long long int hash = cryptHash(pwd, salt);

        // Add the encrypted data record to the database vector
        database.push_back({pwd, salt, hash});

        // Print the original password to the console
        cout << "Password: " << pwd << "\n";
        // Print the generated salt value in decimal
        cout << "Salt (Dec): " << salt << "\n";
        // Print the resulting hash in hexadecimal format
        cout << "Final Hash (Hex): " << hex << uppercase << setw(16) << setfill('0') << hash << dec << "\n";
        // Print a visual separator line
        cout << "------------------------------------------\n";
    }

    // Print header for the verification phase of known passwords
    cout << "\n--- Defined Passwords Verification ---\n";
    // Initialize successful verification counter to zero
    int successCount = 0;

    // Iterate through all records stored in the database vector
    for (int i = 0; i < database.size(); ++i) {
        // Retrieve reference to the current record using explicit type
        const EncryptedData& record = database[i];
        // Print verification attempt status including the password string
        cout << "[" << i + 1 << "] Verifying '" << record.password << "': ";
        
        // Execute verification check using the password, stored hash, and salt
        if (verifyPassword(record.password, record.hash, record.salt)) {
            // Print success message if hashes match
            cout << " Success\n";
            // Increment the success counter
            successCount++;
        } 
        // If the verification check fails
        else {
            // Print failure message
            cout << " Failed\n";
        }
    }
    // Print the final summary of matched versus total passwords
    cout << "Summary: " << successCount << "/" << database.size() << " defined passwords matched successfully.\n";

    // Print header for testing unauthorized or incorrect password entries
    cout << "\n--- Undefined Passwords Verification ---\n";
    
    // Initialize a vector of strings that are not in the database
    vector<string> undefinedEntries = {"wrong_pass", "admin", "guest123", "password", "12345678"};
    
    // Iterate through the list of invalid entries
    for (int i = 0; i < undefinedEntries.size(); ++i) {
        // Calculate a pseudo-random index to test against different database records
        int recordIndex = i % database.size();
        // Retrieve the current testing input string
        string testInput = undefinedEntries[i];
        
        // Print testing status against a specific stored hash index
        cout << "[" << i + 1 << "] Testing input '" << testInput << "' against hash #" << recordIndex + 1 << ": ";
        
        // Check if the incorrect input matches the targeted hash record
        if (verifyPassword(testInput, database[recordIndex].hash, database[recordIndex].salt)) {
            // Print success message (unexpected for invalid inputs)
            cout << " Success\n";
        } 
        // If the hashes do not match as expected
        else {
            // Print failed status for the incorrect password
            cout << " Failed\n";
        }
    }

    // Return zero indicating successful program termination
    return 0;
}