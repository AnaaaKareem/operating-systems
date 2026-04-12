#include "bits.h"

// Function to set a specific bit within an integer container
void setBit(int &container, int bitState, int bitPosition) {
    // Initialize bit mask based on the 32-bit position offset
    unsigned int bitMask = 1U << (32 - bitPosition);

    // Check if the bit state is intended to be 1
    if (bitState) {
        // Apply bit mask using the OR operator to set the bit
        container = container | bitMask;
    } 
    // If the bit state is intended to be 0
    else {
        // Apply bit mask using AND and NOT operators to clear the bit
        container = container & ~bitMask;
    }
}

// Function to retrieve the value of a bit at a specific position
int getBit(int container, int bitPosition) {
    // Right shift the container and apply mask to extract the target bit
    return ((unsigned int)container >> (32 - bitPosition)) & 1;
}

// Function to calculate the integer value from a range of bits
int getBitsValue(int sourceValue, int startPos, int endPos) {
    // Initialize accumulated value and power of two tracking variables
    int accumulatedValue = 0;
    int powerOfTwo = 0;

    // Iterate through the bit range from the end position to the start position
    for (int i = endPos; i >= startPos; i--) {
        // Retrieve the current bit value using the getBit function
        int currentBit = getBit(sourceValue, i);
        // Add the bit's weighted value to the accumulator using bitwise shifting
        accumulatedValue += (currentBit << powerOfTwo);
        // Increment the power of two for the next bit position
        powerOfTwo += 1;
    }

    // Return the total accumulated integer value
    return accumulatedValue;
}

// Function to perform a circular left shift on a bit segment
int circularLeftShift(int keyHalf, int bitWidth, int shiftCount) {
    // Iterate through the total number of required shifts
    for (int i = 0; i < shiftCount; i++) {

        // Capture the carry bit from the most significant position
        int carryBit = getBit(keyHalf, 1);

        // Position the carry bit to be appended at the end of the bit width
        carryBit = carryBit << (32 - bitWidth);

        // Perform the left shift and integrate the carry bit using the OR operator
        keyHalf = (keyHalf << 1) | carryBit;
    }
    
    // Return the modified key half after all shifts are completed
    return keyHalf;
}