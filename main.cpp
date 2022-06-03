#include <stdio.h>

#include "Descriptors/usb_cdc_descriptors.hpp"

int main()
{
  printf("Device descriptor %i bytes:\n", sizeof(Device_Descriptor));
  for(auto &x : Device_Descriptor.buf) 
    printf("%.2X ", x);

  printf("\nConfiguration descriptor %i bytes:\n", sizeof(ConfigDescriptor));
  for(auto &x : ConfigDescriptor.buf) 
    printf("%.2X ", x);
  
  return 0;
}
