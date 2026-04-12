// Check if CRYPT_H is not defined
#ifndef CRYPT_H
// Define CRYPT_H to prevent multiple inclusions
#define CRYPT_H

// Include string library for standard string objects
#include <string>

// Use the standard namespace
using namespace std;

// Function prototype to convert a string password into a 64-bit key
long long int passwordToKey(const string& password);

// Function prototype to generate a cryptographic hash using password and salt
long long int cryptHash(const string& password, int salt);

// Function prototype to verify a password against a stored hash value
bool verifyPassword(const string& password, long long int storedHash, int salt);

// End of CRYPT_H header guard
#endif