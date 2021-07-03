#include <stdio.h>

int valueinarray(int val, int arr[], int num_elem)
{
    int i;
    for(i = 0; i < num_elem; i++)
    {
        if(arr[i] == val){
          return 1;
        }

    }
    return 0;
}

int main(){
  int array0[] = {0,1,2,3,4};
  if(valueinarray(4, array0, sizeof(array0)/sizeof(array0[0]))){
    printf("in array\n");
  }
  else{
    printf("not in array\n");
  }
}
