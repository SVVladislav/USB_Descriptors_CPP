#include <stdio.h>

#define CUSTOM_HID
//#define CDC
//#define CDCx2
//#define WIN_USB
//#define MSD


#ifdef CUSTOM_HID
#include "Descriptors/usb_hid_descriptors.hpp"
#endif

#ifdef CDC
#include "Descriptors/usb_cdc_descriptors.hpp"
#endif

#ifdef CDCx2
#include "Descriptors/usb_2cdc_descriptors.hpp"
#endif

#ifdef WIN_USB
#include "Descriptors/usb_winusb_descriptors.hpp"
#endif

#ifdef MSD
#include "Descriptors/usb_msd_descriptors.hpp"
#endif

int main()
{
  printf("Device descriptor %i bytes:\n", sizeof(Device_Descriptor));
  for(auto &x : Device_Descriptor.buf) 
    printf("%.2X ", x);

  printf("\nConfiguration descriptor %i bytes:\n", sizeof(Configuration_Descriptor));
  for(auto &x : Configuration_Descriptor.buf) 
    printf("%.2X ", x);

#ifdef CUSTOM_HID
    printf("\nHID Report descriptor %i bytes:\n", sizeof(HidReportDescriptor));
  for(auto &x : HidReportDescriptor.buf) 
    printf("%.2X ", x);
#endif
  
  return 0;
}

extern "C" size_t __write(int handle, const unsigned char *buf, size_t bufSize) { return 0; }