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
#define BITMASK 0xFFFFFFF0
#define MAX(a,b) (((a)>(b))?(a):(b))
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

int16_t ieee_to_16(float ieee_input){
  printf("ieee_to_16 function:\n");
  // Instantiate the union
  myfloat var;
  int16_t final_16_bit = 0; // where we will store 16 bit value
  int mantissa_bits = 0, signed_bit = 0, exp_bits = 0;
  var.f = ieee_input;

  if(ieee_input == 0){ // handles case of a 0
    return final_16_bit;
  }

  signed_bit = var.raw.sign; // get signed bit

  exp_bits = (int)var.raw.exponent - 127; // get exponent bits and subtract the bias
  exp_bits = MAX(-16, exp_bits); // count when this happens in darknet
  exp_bits = exp_bits & 0x1F; // AND with 0xF to clear any excess bits
  printf("The original exponent with bias is: %d\n", var.raw.exponent);
  printf("Exponent without bias is: %d\n", exp_bits);
  printf("Exponent bits without bias: ");
  printBinary(exp_bits, 5);
  printf("\n");

  mantissa_bits = var.raw.mantissa >> (23 - 10); // get mantissa bits

  final_16_bit = final_16_bit | signed_bit << 15; // shift sign bit to left most bit and OR with final_16_bit
  final_16_bit = final_16_bit | exp_bits << 10; // shift over 11 bits (number of mantissa bits)
  final_16_bit = final_16_bit | mantissa_bits; // manitssa bits are already right-most, dont need to shift

  return final_16_bit;
}

myfloat int16_to_ieee(int16_t convert_input){
  printf("int16_to_ieee function:\n");
  myfloat ieee;
  int mantissa_bits = 0, signed_bit = 0, exp_bits = 0;
  ieee.f = 0;

  ieee.raw.sign = convert_input >> 15; // get signed bit, shift to rightmost place

  exp_bits = (convert_input & 0x7C00) >> 10; // get exp_bits by AND with 0111110000000000 and shift to rightmost place
  printf("Exponent without bias is: %d\n", exp_bits);
  printf("Exponent bits without bias: ");
  printBinary(exp_bits, 5);
  printf("\n");

  if(exp_bits >> 4 != 0){ // if number is negative
    exp_bits = exp_bits | BITMASK; // fill in left bits with 1 to keep number negative
  }
  //4.0000000567
  if((convert_input & 0x7FF) << (23 - 10) == 0 && exp_bits == 0){ // handles the case of a 0
    ieee.raw.exponent = 0; // dont add bias if the number is 0
  }
  else{
    printf("triggered\n");
    ieee.raw.exponent = 127 + exp_bits; // add back in the bias
  }
  printf("Exponent with bias is: %d\n", ieee.raw.exponent);
  printf("Exponent bits with bias: ");
  printBinary(ieee.raw.exponent, 8);
  printf("\n");

  ieee.raw.mantissa = (convert_input & 0x7FF) << (23 - 10); // get mantissa bits 0 0000 11111111111 and shift over
  printf("Mantissa with mask: ");
  printBinary(ieee.raw.mantissa, 23);
  printf("\n");

  printIEEE(ieee);
  printf("\n");

  return ieee;

}

int8_t ieee_to_8(float ieee_input){
  printf("ieee_to_8 function:\n");
  // Instantiate the union
  myfloat var;
  int8_t final_8_bit = 0; // where we will store 16 bit value
  int mantissa_bits = 0, signed_bit = 0, exp_bits = 0;
  var.f = ieee_input;

  if(ieee_input == 0){ // handles case of a 0
    return final_8_bit;
  }

  signed_bit = var.raw.sign;

  exp_bits = (int)var.raw.exponent - 127; // get exponent bits and subtract the bias
  exp_bits = MAX(-16, exp_bits); // count when this happens in darknet
  printf("exp_bits before mask: %d\n", exp_bits);
  exp_bits = exp_bits & 0x1F; // AND with 0x1F to clear any excess bits
  printf("The original exponent with bias is: %d\n", var.raw.exponent);
  printf("Exponent without bias is: %d\n", exp_bits);
  printf("Exponent bits without bias: ");
  printBinary(exp_bits, 5);
  printf("\n");

  mantissa_bits = var.raw.mantissa >> (23 - 2); // get mantissa bits

  final_8_bit = final_8_bit | signed_bit << 7; // shift sign bit to left most bit and OR with final_16_bit
  final_8_bit = final_8_bit | exp_bits << 2; // shift over 2 bits
  final_8_bit = final_8_bit | mantissa_bits; // manitssa bits are already right-most, dont need to shift

  return final_8_bit;
}

myfloat int8_to_ieee(int8_t convert_input){
  printf("int8_to_ieee function:\n");
  myfloat ieee;
  int mantissa_bits = 0, signed_bit = 0, exp_bits = 0;
  ieee.f = 0;

  ieee.raw.sign = convert_input >> 7; // get signed bit, shift to rightmost place

  exp_bits = (convert_input & 0x7F) >> 2; // get exp_bits by AND with 01111000and shift to rightmost place
  printf("Exponent without bias is: %d\n", exp_bits);
  printf("Exponent bits without bias: ");
  printBinary(exp_bits, 5);
  printf("\n");

  if(exp_bits >> 4 != 0){ // if number is negative 01111100
    exp_bits = exp_bits | BITMASK; // fill in left bits with 1 to keep number negative
  }

  if((convert_input & 0x7) << (23 - 2) == 0 && exp_bits == 0){ // handles the case of a 0
    ieee.raw.exponent = 0; // dont add bias if the number is 0
  }
  else{
    ieee.raw.exponent = 127 + exp_bits; // add back in the bias
  }

  printf("Exponent with bias is: %d\n", ieee.raw.exponent);
  printf("Exponent bits with bias: ");
  printBinary(ieee.raw.exponent, 8);
  printf("\n");

  ieee.raw.mantissa = (convert_input & 0x7) << (23 - 2); // get mantissa bits 0 0000 11111111111 and shift over
  printf("Mantissa with mask: ");
  printBinary(ieee.raw.mantissa, 23);
  printf("\n");

  printIEEE(ieee);
  printf("\n");

  return ieee;
}
// Driver Code
int main()
{
    float input_var = 0;

    // Get the input variable
    printf("Enter IEEE 754 number: ");
    scanf("%f", &input_var); // number we want to change to new form and back to IEEE 754

    // Get the IEEE floating point representation
    // for debug purposes only
    myfloat ieee_cast;
    ieee_cast.f = input_var;
    printf("IEEE 754 representation of %f is : \n", ieee_cast.f);
    printIEEE(ieee_cast);
    printf("\n");

    // convert to 8 bit
    int8_t convert_from_8;
    convert_from_8 = ieee_to_8(input_var);
    printf("8 bit representation: ");
    printBinary(convert_from_8, 8);
    printf("\n\n");

    // convert form 8 bit to IEEE 754
    myfloat convert_back_8 = int8_to_ieee(convert_from_8);
    printf("Back to IEEE value from 8 bit: %f\n\n", convert_back_8.f);

    // convert to 16 bit
    int16_t convert_from;
    convert_from = ieee_to_16(input_var);
    printf("16 bit IEEE format: ");
    printBinary(convert_from, 16);
    printf("\n\n");

    // convert from 16 bit to IEEE 754
    myfloat convert_back = int16_to_ieee(convert_from);
    printf("Back to IEEE value from 16 bit: %f\n", convert_back.f);

    return 0;
}
