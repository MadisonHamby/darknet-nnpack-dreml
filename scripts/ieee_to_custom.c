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
    /*
    // cut down to:
    // 0 | 1000 | 100
    int signed_bit = var.raw.sign; // get signed bit
    printf("Signed bit: ");
    printBinary(signed_bit, 1);
    printf("\n");

    int exp_bits;
    printf("The original exponent with bias is: %d\n", var.raw.exponent);
    exp_bits = abs(var.raw.exponent - 127);
    printf("Exponent without bias is: %d\n", exp_bits);
    printf("Exponent bits without bias: ");
    printBinary(exp_bits, 4);
    printf("\n");

    int mantissa_bits = var.raw.mantissa >> (23 - 3);
    // get mantissa bits (shift over 23 - # of mantissa bits)
    printf("Mantissa bits: ");
    printBinary(mantissa_bits, 3);
    printf("\n");

    int final_8_bit = 0; // where we will store final 8 bit
    final_8_bit = final_8_bit | signed_bit << 7; // OR with 0
    //printBinary(final_8_bit, 8);
    //printf("\n");
    final_8_bit = final_8_bit | exp_bits << 3;
    //printBinary(final_8_bit, 8);
    //printf("\n");
    final_8_bit = final_8_bit | mantissa_bits;
    printf("8 bit reresentation: ");
    printBinary(final_8_bit, 8);
    printf("\n");

    // convert back to IEEE
    myfloat ieee;
    ieee.f = 0;
    ieee.raw.sign = final_8_bit >> 7;
    // add back in the bias
    // add back in the bias
    ieee.raw.exponent = 127 - exp_bits;
    ieee.raw.mantissa = (final_8_bit & 0xF) << (23 - 3);
    printf("Back to IEEE: %lf\n", ieee.f);

    printIEEE(ieee);
    */
    myfloat ieee;
    int mantissa_bits, signed_bit, exp_bits;
    // Now we will change to 16 bit representation
    printf("\n\n");
    printf("16 bit representation\n");
    int exp_array[5];
    // 16 bit format
    // 1 signed bit
    // 4 exponent bits
    // 11 mantissa bits

    // signed bit
    signed_bit = var.raw.sign; // get signed bit
    printf("Signed bit: ");
    printBinary(signed_bit, 1);
    printf("\n");

    // exponent bits
    printf("The original exponent with bias is: %d\n", var.raw.exponent);
    // if number is negative
    exp_array[0] = var.raw.exponent - 127;

    printf("Exponent without bias is: %d\n", exp_array[0]);
    printf("Exponent bits without bias: ");
    printBinary(exp_array[0], 4);
    printf("\n");

    // mantissa bits
    mantissa_bits = var.raw.mantissa >> (23 - 11);
    // get mantissa bits (shift over 23 - # of mantissa bits)
    printf("Mantissa bits: ");
    printBinary(mantissa_bits, 11);
    printf("\n");

    int16_t final_16_bit = 0; // where we will store final 16 bit
    final_16_bit = final_16_bit | signed_bit << 15; // OR with 0
    final_16_bit = final_16_bit | exp_array[0] << 11; // shift over number of mantissa bits
    final_16_bit = final_16_bit | mantissa_bits;
    printf("16 bit representation: ");
    printBinary(final_16_bit, 16);
    printf("\n");


    // convert back to IEEE
    ieee.f = 0;
    //ieee.raw.sign = signed_bit;
    ieee.raw.sign = final_16_bit >> 15; // get signed bit 1 0000 00000000000
    // add back in the bias
    // if number is negative
    ieee.raw.exponent = 127 + exp_array[0];
    // if number is positive 127 + number
    ieee.raw.mantissa = (final_16_bit & 0x7FF) << (23 - 11); // get mantissa bits 0 0000 11111111111
    printf("Back to IEEE: %lf\n", ieee.f);

    printIEEE(ieee);





    return 0;
}
