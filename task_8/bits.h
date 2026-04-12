// Check if BITS_H is not defined
#ifndef BITS_H
// Define BITS_H to prevent multiple inclusions
#define BITS_H

// Function prototype to set a bit state at a specific position
void setBit(int &container, int bitState, int bitPosition);

// Function prototype to retrieve a bit value from a container
int getBit(int container, int bitPosition);

// Function prototype to calculate the integer value of a bit range
int getBitsValue(int sourceValue, int startPos, int endPos);

// Function prototype to perform a circular left shift on a bit segment
int circularLeftShift(int keyHalf, int bitWidth, int shiftCount);

// End of BITS_H header guard
#endif