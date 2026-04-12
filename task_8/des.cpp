#include "des.h"

// Function to perform Permuted Choice 1 (PC-1) to reduce 64-bit key to 56 bits
void permuteKeyPC1(int &keyLeftHalf, int &keyRightHalf) {

    // Initialize the PC-1 permutation table with source bit positions
    int pc1Table[56] = {
        57, 49, 41, 33, 25, 17, 9,  1,  58, 50, 42, 34, 26, 18, 10, 2,  59, 51, 43, 35, 27, 19, 11, 3,  60, 52, 44, 36,
        63, 55, 47, 39, 31, 23, 15, 7,  62, 54, 46, 38, 30, 22, 14, 6,  61, 53, 45, 37, 29, 21, 13, 5,  28, 20, 12, 4
    };

    // Initialize new left and right halves to zero
    int nextLeft = 0;
    int nextRight = 0;

    // Iterate through all 56 positions of the PC-1 table
    for (int i = 0; i < 56; i++) {
        // Retrieve the source bit position from the table
        int sourceBitPosition = pc1Table[i];

        // Check if the source bit originates from the right half (bit 33-64)
        if (sourceBitPosition > 32) {

            // Extract the bit value using the 32-bit offset for the right half
            int bitValue = getBit(keyRightHalf, sourceBitPosition - 32);

            // Check if the current target position belongs to the first 28 bits
            if (i < 28) {
                // Set the bit in the next left half
                setBit(nextLeft, bitValue, i + 1);
            } 
            // If the target position belongs to the remaining 28 bits
            else {
                // Set the bit in the next right half
                setBit(nextRight, bitValue, i - 27);
            }
        } 
        // If the source bit originates from the left half (bit 1-32)
        else {

            // Extract the bit value from the left half
            int bitValue = getBit(keyLeftHalf, sourceBitPosition);

            // Check if the current target position belongs to the first 28 bits
            if (i < 28) {
                // Set the bit in the next left half
                setBit(nextLeft, bitValue, i + 1);
            } 
            // If the target position belongs to the remaining 28 bits
            else {
                // Set the bit in the next right half
                setBit(nextRight, bitValue, i - 27);
            }
        }
    }

    // Update the key halves and mask to ensure only 28 bits are stored (left-aligned)
    keyRightHalf = nextRight & 0b11111111111111111111111111110000;
    keyLeftHalf = nextLeft & 0b11111111111111111111111111110000;
}

// Function to perform Permuted Choice 2 (PC-2) to generate 48-bit subkeys
void permuteSubkeyPC2(int &internalLeftHalf, int &internalRightHalf) {

    // Initialize the PC-2 permutation table with source bit positions
    int pc2Table[48] = {
        14, 17, 11, 24, 1,  5,  3,  28, 15, 6,  21, 10, 23, 19, 12, 4,  26, 8,  16, 7,  27, 20, 13, 2,
        41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48, 44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
    };

    // Initialize subkey parts to zero
    int subkeyPart1 = 0;
    int subkeyPart2 = 0;

    // Iterate through all 48 positions of the PC-2 table
    for (int i = 0; i < 48; i++) {
        // Retrieve the source bit position from the table
        int sourceBitPosition = pc2Table[i];

        // Check if the source bit originates from the internal right half (bit 29-56)
        if (sourceBitPosition > 28) {

            // Extract the bit value using the 28-bit offset for the right half
            int bitValue = getBit(internalRightHalf, sourceBitPosition - 28);

            // Check if the target position belongs to the second half of the subkey
            if (i > 23) {
                // Set the bit in the second subkey part
                setBit(subkeyPart2, bitValue, i - 23);
            } 
            // If the target position belongs to the first half of the subkey
            else {
                // Set the bit in the first subkey part
                setBit(subkeyPart1, bitValue, i + 1);
            }
        } 
        // If the source bit originates from the internal left half (bit 1-28)
        else {

            // Extract the bit value from the left half
            int bitValue = getBit(internalLeftHalf, sourceBitPosition);

            // Check if the target position belongs to the second half of the subkey
            if (i > 23) {
                // Set the bit in the second subkey part
                setBit(subkeyPart2, bitValue, i - 23);
            } 
            // If the target position belongs to the first half of the subkey
            else {
                // Set the bit in the first subkey part
                setBit(subkeyPart1, bitValue, i + 1);
            }
        }
    }

    // Update the internal halves and mask to store 24 bits each (left-aligned)
    internalRightHalf = subkeyPart2 & 0b11111111111111111111111100000000;
    internalLeftHalf = subkeyPart1 & 0b11111111111111111111111100000000;
}

// Function to perform Initial Permutation (IP) on the 64-bit plaintext block
void initialPermutation(int &leftHalf, int &rightHalf) {

    // Initialize the IP table with 64 bit positions
    int initialPermutationTable[64] = {
        58,50,42,34,26,18,10,2,60,52,44,36,28,20,12,4,62,54,46,38,30,22,14,6,64,56,48,40,32,24,16,8,
        57,49,41,33,25,17,9,1,59,51,43,35,27,19,11,3,61,53,45,37,29,21,13,5,63,55,47,39,31,23,15,7
    };

    // Initialize next state halves to zero
    int nextLeft = 0;
    int nextRight = 0;

    // Iterate through all 64 positions of the permutation table
    for (int i = 0; i < 64; i++) {
        // Retrieve source bit index from the table
        int sourceBitIndex = initialPermutationTable[i];

        // Check if source bit is from the original right half
        if (sourceBitIndex > 32) {
            // Extract bit using offset
            int bitValue = getBit(rightHalf, sourceBitIndex - 32);

            // Determine if bit belongs to the new right or left half
            if (i > 31) setBit(nextRight, bitValue, i - 31);
            else setBit(nextLeft, bitValue, i + 1);
        } 
        // If source bit is from the original left half
        else {
            // Extract bit directly
            int bitValue = getBit(leftHalf, sourceBitIndex);

            // Determine if bit belongs to the new right or left half
            if (i > 31) setBit(nextRight, bitValue, i - 31);
            else setBit(nextLeft, bitValue, i + 1);
        }
    }

    // Update original halves with permuted values
    rightHalf = nextRight;
    leftHalf = nextLeft;
}

// Function to expand the 32-bit right half to 48 bits and apply salt
void expansionPermutation(int &expandedLeft, int &inputRight, int salt) {

    // Initialize expansion table to map 32 bits to 48 bits
    int expansionTable[48] = {
        32,1,2,3,4,5,4,5,6,7,8,9,8,9,10,11,12,13,12,13,14,15,16,17,
        16,17,18,19,20,21,20,21,22,23,24,25,24,25,26,27,28,29,28,29,30,31,32,1
    };

    // Initialize working buffers to zero
    int workingLeft = 0;
    int workingRight = 0;

    // Iterate through 48 positions to perform expansion
    for (int i = 0; i < 48; i++) {
        // Extract bit value from the 32-bit right half
        int bitValue = getBit(inputRight, expansionTable[i]);

        // Map the expanded bit to the left or right working buffer
        if (i > 23) setBit(workingRight, bitValue, i - 23);
        else setBit(workingLeft, bitValue, i + 1);
    }

    // Apply salt by swapping bits based on the 16-bit salt integer
    for (int bitIndex = 1; bitIndex <= 16; bitIndex++) {
        // Check if the current salt bit is set
        if (salt & (1 << (bitIndex - 1))) {
            // Retrieve bits from both halves to perform swap
            int bitFromLeftHalf = getBit(workingLeft, bitIndex);
            int bitFromRightHalf = getBit(workingRight, bitIndex);

            // Set bits in swapped positions
            setBit(workingLeft, bitFromRightHalf, bitIndex);
            setBit(workingRight, bitFromLeftHalf, bitIndex);
        }
    }

    // Update expanded results and mask to 24 bits each (left-aligned)
    expandedLeft = workingLeft & 0b11111111111111111111111100000000;
    inputRight = workingRight & 0b11111111111111111111111100000000;
}

// Function to perform S-Box substitution to reduce 6-bit input to 4-bit output
int sBoxSubstitution(int sBoxIndex, int row, int column) {

    // Initialize the combined S-Box table containing all 8 boxes (64 entries each)
    int S[512] = {
        14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7, 0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8,
        4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0, 15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13,
        15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10, 3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5,
        0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15, 13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9,
        10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8, 13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1,
        13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7, 1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12,
        7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15, 13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9,
        10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4, 3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14,
        2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9, 14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6,
        4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14, 11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3,
        12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11, 10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8,
        9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6, 4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13,
        4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1, 13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6,
        1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2, 6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12,
        13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7, 1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2,
        7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8, 2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11
    };

    // Calculate the unique lookup index based on S-Box ID, row, and column
    int lookupIndex = (sBoxIndex - 1) * 64 + row * 16 + column;
    // Return the substituted 4-bit value from the table
    return S[lookupIndex];
}



// Function to perform Final Permutation (IP-1) on the 64-bit encrypted block
void finalPermutation(int &leftHalf, int &rightHalf) {

    // Initialize the inverse initial permutation table (IP-1)
    int inverseInitialPermutationTable[64] = {
        40,8,48,16,56,24,64,32,39,7,47,15,55,23,63,31,38,6,46,14,54,22,62,30,37,5,45,13,53,21,61,29,36,4,44,12,
        52,20,60,28,35,3,43,11,51,19,59,27,34,2,42,10,50,18,58,26,33,1,41,9,49,17,57,25
    };

    // Initialize buffers for the permuted halves
    int permutedLeft = 0;
    int permutedRight = 0;

    // Iterate through all 64 positions of the inverse table
    for (int i = 0; i < 64; i++) {
        // Retrieve source bit index from the inverse table
        int sourceBitIndex = inverseInitialPermutationTable[i];

        // Check if the source bit originates from the final right half
        if (sourceBitIndex > 32) {
            // Extract bit using the 32-bit offset
            int bitValue = getBit(rightHalf, sourceBitIndex - 32);

            // Assign the bit to the new right or left permuted buffer
            if (i > 31) {
                setBit(permutedRight, bitValue, i - 31);
            } else {
                setBit(permutedLeft, bitValue, i + 1);
            }
        } 
        // If the source bit originates from the final left half
        else {
            // Extract the bit directly
            int bitValue = getBit(leftHalf, sourceBitIndex);

            // Assign the bit to the new right or left permuted buffer
            if (i > 31) {
                setBit(permutedRight, bitValue, i - 31);
            } else {
                setBit(permutedLeft, bitValue, i + 1);
            }
        }
    }

    // Update the final halves with the permuted results
    rightHalf = permutedRight;
    leftHalf = permutedLeft;
}

// Function to perform the fixed P-Box permutation on S-Box outputs
int pBoxPermutation(int sBoxOutput) {

    // Initialize the P-Box table for 32-bit permutation
    int pBoxTable[32] = {16,7,20,21,29,12,28,17,1,15,23,26,5,18,31,10,2,8,24,14,32,27,3,9,19,13,30,6,22,11,4,25};
    // Initialize results buffer to zero
    int permutedResult = 0;

    // Iterate through all 32 positions of the P-Box table
    for (int i = 0; i < 32; i++) {
        // Extract bit from S-Box output and set it in the target position
        setBit(permutedResult, getBit(sBoxOutput, pBoxTable[i]), i + 1);
    }
    // Return the permuted 32-bit integer
    return permutedResult;
}

// Function to execute the Feistel round function (F-function)
int feistelFunction(int rightHalf, int subkeyPart1, int subkeyPart2, int salt) {
    // Initialize variables for expansion
    int expandedPart1 = 0;
    int expandedPart2 = rightHalf;

    // Expand the 32-bit right half to 48 bits using expansion permutation
    expansionPermutation(expandedPart1, expandedPart2, salt);

    // XOR the expanded data with the 48-bit subkey
    subkeyPart1 ^= expandedPart1;
    subkeyPart2 ^= expandedPart2;

    // Initialize the combined S-Box output accumulator
    int sBoxAccumulatedOutput = 0;

    // Iterate through all 8 S-Boxes to process 6-bit chunks
    for (int i = 8; i > 0; i--) {
        // Initialize buffer for the current 6-bit input chunk
        int sBoxInputChunk = 0;

        // Extract 6-bit chunks from subkey parts based on S-Box index
        if (i > 4) sBoxInputChunk = getBitsValue(subkeyPart2, i * 6 - 24 - 5, i * 6 - 24);
        else sBoxInputChunk = getBitsValue(subkeyPart1, i * 6 - 5, i * 6);

        // Calculate S-Box column using the inner 4 bits
        int column = getBitsValue(sBoxInputChunk, 28, 31);
        // Calculate S-Box row using the outer 2 bits (1st and 6th bits)
        int row = getBitsValue(getBitsValue(sBoxInputChunk, 32, 32) | (getBitsValue(sBoxInputChunk, 27, 27) << 1), 30, 32);

        // Accumulate the 4-bit S-Box output into the 32-bit result
        sBoxAccumulatedOutput += sBoxSubstitution(i, row, column) << ((8 - i) * 4);
    }

    // Return the result after applying the final P-Box permutation
    return pBoxPermutation(sBoxAccumulatedOutput);
}

// Function to encrypt a single 64-bit plaintext block using DES
long long int desEncryptBlock(long long int plaintextBlock, long long int originalKey, int salt) {
    // Split the 64-bit key into two 32-bit halves
    int keyRight = (int)(originalKey);
    int keyLeft = (int)(originalKey >> 32);

    // Apply Permuted Choice 1 to the key
    permuteKeyPC1(keyLeft, keyRight);

    // Initialize internal key halves for the round schedule
    int internalKeyLeft = keyLeft;
    int internalKeyRight = keyRight;

    // Split the 64-bit plaintext into two 32-bit halves
    int currentRightHalf = (int)(plaintextBlock);
    int currentLeftHalf = (int)(plaintextBlock >> 32);

    // Execute the Initial Permutation (IP)
    initialPermutation(currentLeftHalf, currentRightHalf);

    // Iterate through all 16 rounds of the Feistel network
    for (int round = 1; round <= 16; round++) {

        // Determine left-shift amount based on the current round number
        int shiftAmount = (round == 1 || round == 2 || round == 9 || round == 16) ? 1 : 2;

        // Perform circular left shift on both key halves
        internalKeyLeft = circularLeftShift(internalKeyLeft, 28, shiftAmount);
        internalKeyRight = circularLeftShift(internalKeyRight, 28, shiftAmount);

        // Generate the 48-bit subkey for the current round using PC-2
        int subkeyPart1 = internalKeyLeft;
        int subkeyPart2 = internalKeyRight;
        permuteSubkeyPC2(subkeyPart1, subkeyPart2);

        // Execute the Feistel round logic and update halves
        int previousLeft = currentLeftHalf;
        currentLeftHalf = currentRightHalf;
        currentRightHalf = previousLeft ^ feistelFunction(currentRightHalf, subkeyPart1, subkeyPart2, salt);
    }

    // Perform final swap of the left and right halves before final permutation
    int tempSwap = currentRightHalf;
    currentRightHalf = currentLeftHalf;
    currentLeftHalf = tempSwap;

    // Execute the Final Permutation (IP-1)
    finalPermutation(currentLeftHalf, currentRightHalf);

    // Reassemble the two 32-bit halves into a single 64-bit encrypted block
    long long int encryptedResult = (unsigned int)currentLeftHalf;
    encryptedResult = (encryptedResult << 32) + (unsigned int)currentRightHalf;

    // Return the final encrypted 64-bit result
    return encryptedResult;
}