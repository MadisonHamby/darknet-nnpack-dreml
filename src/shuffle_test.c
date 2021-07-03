#include <stdio.h>
#include <stdint.h>

#define MAX(a,b) (((a)>(b))?(a):(b))
#define BITMASK 0xFFFFFFF0

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

int16_t ieee_to_16(float ieee_input){
  // Instantiate the union
  myfloat var;
  int16_t final_16_bit = 0; // where we will store 16 bit value
  int mantissa_bits = 0, signed_bit = 0, exp_bits = 0;
  var.f = ieee_input;

  signed_bit = var.raw.sign; // get signed bit

  exp_bits = (int)var.raw.exponent - 127; // get exponent bits and subtract the bias
  exp_bits = MAX(-16, exp_bits); // count when this happens in darknet
  exp_bits = exp_bits & 0x1F; // AND with 0xF to clear any excess bits

  mantissa_bits = var.raw.mantissa >> (23 - 10); // get mantissa bits

  final_16_bit = final_16_bit | signed_bit << 15; // shift sign bit to left most bit and OR with final_16_bit
  final_16_bit = final_16_bit | exp_bits << 10; // shift over 11 bits (number of mantissa bits)
  final_16_bit = final_16_bit | mantissa_bits; // manitssa bits are already right-most, dont need to shift

  return final_16_bit;
}

myfloat int16_to_ieee(int16_t convert_input){
  myfloat ieee;
  int mantissa_bits = 0, signed_bit = 0, exp_bits = 0;
  ieee.f = 0;

  ieee.raw.sign = convert_input >> 15; // get signed bit, shift to rightmost place

  exp_bits = (convert_input & 0x7C00) >> 10; // get exp_bits by AND with 0111110000000000 and shift to rightmost place

  if(exp_bits >> 4 != 0){ // if number is negative
    exp_bits = exp_bits | BITMASK; // fill in left bits with 1 to keep number negative
  }

  if((convert_input & 0x7FF) << (23 - 10) != 0){ // handles the case of a 0
    ieee.raw.exponent = 127 + exp_bits; // add back in the bias
  }
  else{
    ieee.raw.exponent = 0; // dont add bias if the number is 0
  }

  ieee.raw.mantissa = (convert_input & 0x7FF) << (23 - 10); // get mantissa bits 0 0000 11111111111 and shift over

  return ieee;

}

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

int main(){
  /*
  int16_t array0;
  char array2[5];
  float input_var, output_var;
  myfloat ieee_array;
  input_var = 5.6789;
  // rearrange exponent and mantissa bytes to improve compression
  array0 = ieee_to_16(input_var);
  printf("16 bit input_var:\n");
  printBinary(array0, 16);
  printf("\n");
  array2[0] = array0 & 0x00FF; // last byte
  printf("mantissa bits for 16 bit:\n");
  printBinary(array2[0], 8);
  printf("\n");
  array2[2] = (array0 & 0xFF00) >> 8; // first byte, shift over to the right 1 byte
  printf("exponent and signed bits for 16 bit:\n");
  printBinary(array2[2], 8);
  printf("\n");


  // rearrange exponent and mantissa bytes back to improve compression
  array0 = (array2[0] & 0x00FF) | (array2[2] << 8); // get last byte, or with first byte shifted over to leftmost byte
  //array0 = array2[0] | (array2[2] << 7); // shift back beginning byte and OR with last byte
  printf("array0 is:\n");
  printBinary(array0, 16);
  printf("\n");
  ieee_array = int16_to_ieee(array0);
  output_var = ieee_array.f;
  printf("output_var is: %f\n", output_var);
  */

  char array0[4]; // 4 bytes of memory
  float input_var = 3.567;
  int output_var; // float is 4 bytes, this is l.output
  int before_sep; // need to convert float to binary
  myfloat ieee_num; // myfloat union
  ieee_num.f = input_var; // set float in union equal to input_var
  before_sep = ieee_num.raw.sign << 31 | ieee_num.raw.exponent << 23 | ieee_num.raw.mantissa; // 32 bit ieee format
  printf("Input variable:\n");
  printBinary(before_sep, 32);
  printf("\n");

  // seperate first byte
  array0[0] = (before_sep & 0xFF000000) >> 24;
  printf("Seperate first byte:\n");
  printBinary(array0[0], 8);
  printf("\n");
  // seperate last 3 bytes
  int bytes_3 = (before_sep & 0x00FFFFFF);
  array0[3] = bytes_3;
  array0[2] = bytes_3 >> 8;
  array0[1] = bytes_3 >> 16;
  printf("Seperate last 3 bytes:\n");
  printBinary(array0[1], 8);
  printBinary(array0[2], 8);
  printBinary(array0[3], 8);
  printf("\n");

  // put back together in output_var
  // put first byte back
  output_var = array0[0] << 24;
  // put last three bytes back
  output_var = output_var | (array0[1] & 0xFF) << 16;
  output_var = output_var | (array0[2] & 0xFF) << 8;
  output_var = output_var | (array0[3] & 0xFF);

  printBinary(output_var, 32);
  printf("\n");
   // convert back to float
  ieee_num.raw.sign = (output_var & 0x80000000) >> 31;
  printBinary(ieee_num.raw.sign, 1);
  printf("\n");
  ieee_num.raw.exponent = (output_var & 0x7F800000) >> 23;
  printBinary(ieee_num.raw.exponent, 8);
  printf("\n");
  ieee_num.raw.mantissa = output_var & 0x007FFFFF;
  printBinary(ieee_num.raw.mantissa, 23);
  printf("\n");
  printf("Output var back:\n");
  printBinary(output_var, 32);
  printf("\n");
  printf("output_var is %f\n", ieee_num.f);


}
