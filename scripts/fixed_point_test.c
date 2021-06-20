#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <time.h>
/// Fixed-point Format: 11.5 (16-bit)
typedef int16_t fixed_point_t;

#define FIXED_POINT_FRACTIONAL_BITS 5
//#define ARRAY_SIZE  553779// 5537792

/// Converts 11.5 format -> double
double fixed_to_float(fixed_point_t input)
{
    return ((double)input / (double)(1 << FIXED_POINT_FRACTIONAL_BITS));
}

/// Converts double to 11.5 format
fixed_point_t float_to_fixed(double input)
{
    return (fixed_point_t)(round(input * (1 << FIXED_POINT_FRACTIONAL_BITS)));
}


int main(){
  clock_t float_to_fixed_start, fixed_to_float_start, float_to_fixed_end, fixed_to_float_end;
  unsigned long int array_size = 553779200; // size for first layer
  double *input_array = (double*)malloc(array_size * sizeof(double));
  double *output_array = (double*)malloc(array_size * sizeof(double));
  fixed_point_t *fixed_array = (fixed_point_t*)malloc(array_size * sizeof(fixed_point_t));


  // read csv data
  FILE* fp = fopen("frame_25000_tensors.csv", "r");

  if(fp == NULL){
    printf("File not opened\n");
    return 1;
  }

  for(int i = 0; i < array_size; i++){
    fscanf(fp, "%lf", &input_array[i]);
    //printf("%lf\n",input_array[i]);
  }


  // start float to fixed clock
  float_to_fixed_start = clock();
  for(int i = 0; i < array_size; i++){
    //printf("%lf\n", input_array[i]);
    fixed_array[i] = float_to_fixed(input_array[i]);
  }

  // stop float to fixed clock
  float_to_fixed_end = clock();
  printf("end time - start time for float to fixed: %ld\n", (float_to_fixed_end - float_to_fixed_start));
  printf("float to fixed time: %lf\n", ((double)((float_to_fixed_end - float_to_fixed_start) / CLOCKS_PER_SEC) / 1000));

  // start fixed to float clock
  fixed_to_float_start = clock();
  for(int i = 0; i < array_size; i++){
    output_array[i] = fixed_to_float(fixed_array[i]);
  }
  // stop fixed to float clock
  fixed_to_float_end = clock();
  printf("end time - start time for fixed to float: %ld\n", (fixed_to_float_end - fixed_to_float_start));
  printf("fixed to float time: %lf\n", ((double)((fixed_to_float_end - fixed_to_float_start) / CLOCKS_PER_SEC) / 1000));
  printf("fixed to float - float to fixed: %ld\n", ((fixed_to_float_end - fixed_to_float_start) - (float_to_fixed_end - float_to_fixed_start)));

  fclose(fp);
}
