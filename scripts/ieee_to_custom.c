#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <time.h>
// compile with g++

// Take a number as an input
// Convert number to custom Format
// Print custom formatted number
// Convert number back to IEEE format
// Print that number

// 8bit format
// 1 signed bit
// 4 exponent bits
// 3 mantissa bits
// bias of 7

// 16 bit format
// 1 signed bit
// 4 exponent bits
// 11 mantissa bits

// Print binary format of a number

// C program to convert a real value
// to IEEE 754 floating point representaion

#include <stdio.h>

void printBinary(int n, int i)
{

    // Prints the binary representation
    // of a number n up to i-bits.
    int k;
    for (k = i - 1; k >= 0; k--) {

        if ((n >> k) & 1)
            printf("1");
        else
            printf("0");
    }
}

typedef union {

    float f;
    struct
    {

        // Order is important.
        // Here the members of the union data structure
        // use the same memory (32 bits).
        // The ordering is taken
        // from the LSB to the MSB.
        unsigned int mantissa : 23;
        unsigned int exponent : 8;
        unsigned int sign : 1;

    } raw;
} myfloat;

// Function to convert real value
// to IEEE foating point representation
void printIEEE(myfloat var)
{

    // Prints the IEEE 754 representation
    // of a float value (32 bits)

    printf("%d | ", var.raw.sign);
    printBinary(var.raw.exponent, 8);
    printf(" | ");
    printBinary(var.raw.mantissa, 23);
    printf("\n");
}

// Driver Code
int main()
{

    // Instantiate the union
    myfloat var;

    // Get the real value
    printf("Enter IEEE 754 number: ");
    scanf("%f", &var.f); // number we want to change to 8 bit and back to IEEE 754

    // Get the IEEE floating point representation
    printf("IEEE 754 representation of %f is : \n",
           var.f);
    printIEEE(var);

    // cut down to:
    // 0 | 1000 | 100

    int signed_bit = var.raw.sign; // get signed bit
    // signed bit will be 1000 0000
    // or 0000 0000
    printf("Signed bit: ");
    printBinary(signed_bit, 8);
    printf("\n");

    int exp_bits = var.raw.exponent >> 1;
    // exp_bits ex: if exponent bits are 111
    // Store like 1110 0000 (8 bit number)
    // Right shift over 1 to leave room for signed bit
    // exp_bits = 0111 0000
    printf("Exponent bits: ");
    printBinary(exp_bits, 8);
    printf("\n");

    int mantissa_bits = var.raw.mantissa >> (23 - 3);
    // get mantissa bits (shift over 23 - # of mantissa bits)
    printf("Mantissa bits: ");
    printBinary(mantissa_bits, 8);
    printf("\n");

    int final_8_bit = 0; // where we will store final 8 bit
    final_8_bit = final_8_bit | signed_bit; // OR with 0
    //printBinary(final_8_bit, 8);
    //printf("\n");
    final_8_bit = final_8_bit | exp_bits;
    //printBinary(final_8_bit, 8);
    //printf("\n");
    final_8_bit = final_8_bit | mantissa_bits;
    printf("8 bit reresentation: ");
    printBinary(final_8_bit, 8);
    printf("\n");

    // convert back to IEEE
    myfloat ieee;
    ieee.f = 0;
    ieee.raw.sign = signed_bit;
    ieee.raw.exponent = exp_bits << 1;
    ieee.raw.mantissa = mantissa_bits << (23 - 3);
    printf("Back to IEEE: %lf\n", ieee.f);

    printIEEE(ieee);


    return 0;
}
