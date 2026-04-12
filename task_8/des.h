// Check if DES_H is not defined
#ifndef DES_H
// Define DES_H to prevent multiple inclusions
#define DES_H

// Include bit manipulation utilities for bitwise operations
#include "bits.h"

// Function prototype to perform Permuted Choice 1 (PC-1) on the encryption key
void permuteKeyPC1(int &keyLeftHalf, int &keyRightHalf);

// Function prototype to perform Permuted Choice 2 (PC-2) for subkey generation
void permuteSubkeyPC2(int &internalLeftHalf, int &internalRightHalf);

// Function prototype to execute the Initial Permutation (IP) on the data block
void initialPermutation(int &leftHalf, int &rightHalf);

// Function prototype to expand the 32-bit right half and integrate the salt
void expansionPermutation(int &expandedLeft, int &inputRight, int salt);

// Function prototype to perform S-Box substitution to reduce bit width
int sBoxSubstitution(int sBoxIndex, int row, int column);

// Function prototype to execute the Final Permutation (IP-1) on the data block
void finalPermutation(int &leftHalf, int &rightHalf);

// Function prototype to perform P-Box permutation on the S-Box output
int pBoxPermutation(int sBoxOutput);

// Function prototype to execute the core Feistel function for each round
int feistelFunction(int rightHalf, int subkeyPart1, int subkeyPart2, int salt);

// Function prototype to encrypt a 64-bit block using the full DES algorithm
long long int desEncryptBlock(long long int plaintextBlock, long long int originalKey, int salt);

// End of DES_H header guard
#endif