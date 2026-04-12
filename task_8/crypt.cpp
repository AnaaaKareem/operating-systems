#include "crypt.h"
#include "des.h"
#include <string>

using namespace std;

// Function to convert a string password into a 64-bit long long key
long long int passwordToKey(const string& password) {

    // Initialize the 64-bit key variable to zero
    long long int key = 0;

    // Iterate through exactly 8 character positions to build the key
    for (int i = 0; i < 8; i++) {
        // Initialize local variable to store the character value
        long long int letter;
        
        // Check if the current index is within the actual password length
        if (i < password.length()) {
            // Convert character to unsigned char to prevent sign extension
            letter = (unsigned char)password[i];
        } 
        // If the index exceeds the password length
        else {
            // Set letter to zero to pad the remaining key bytes
            letter = 0;
        }

        // Shift the byte to the correct position (Big-Endian) and add to key
        key += (letter << ((7 - i) * 8));
    }

    // Return the completed 64-bit key
    return key;
}

// Function to generate a cryptographic hash from a password and salt
long long int cryptHash(const string& password, int salt) {

    // Generate a 64-bit key from the input password string
    long long int key = passwordToKey(password);

    // Initialize the data block to be encrypted to zero
    long long int data = 0;

    // Iterate 25 times to perform repeated encryption cycles
    for (int i = 0; i < 25; i++) {

        // Execute a DES encryption block using the current data, key, and salt
        data = desEncryptBlock(data, key, salt);
    }

    // Return the raw 64-bit data value
    return data;
}

// Function to verify a password against a stored cryptographic hash
bool verifyPassword(const string& password, long long int storedHash, int salt) {

    // Re-generate the hash and compare it against the stored value
    return cryptHash(password, salt) == storedHash;
}