#include <stdio.h>

#define CUSTOM_HID

#ifdef CUSTOM_HID
#include "Descriptors/usb_hid_descriptors.hpp"
#else
#include "Descriptors/usb_cdc_descriptors.hpp"
#endif

int main()
{
  printf("Device descriptor %i bytes:\n", sizeof(Device_Descriptor));
  for(auto &x : Device_Descriptor.buf) 
    printf("%.2X ", x);

  printf("\nConfiguration descriptor %i bytes:\n", sizeof(ConfigDescriptor));
  for(auto &x : ConfigDescriptor.buf) 
    printf("%.2X ", x);

#ifdef CUSTOM_HID
    printf("\nHID Report descriptor %i bytes:\n", sizeof(HidReportDescriptor));
  for(auto &x : HidReportDescriptor.buf) 
    printf("%.2X ", x);
#endif
  
  return 0;
}
