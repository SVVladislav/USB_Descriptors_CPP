#pragma once

#if (__cplusplus > 201703L)
#include "C++20/usb_descriptors.hpp"
#else
#include "C++17/usb_descriptors.h"
#endif 

STRING_DESCRIPTOR( 0, StringLangID,    u"\x0409"                );
STRING_DESCRIPTOR( 1, StringVendor,    u"STMicroelectronics"    );
STRING_DESCRIPTOR( 2, StringProduct,   u"STM32 Mass Storage" );
STRING_DESCRIPTOR( 3, StringSerial,    u"00000000001B"          );

inline const uint8_t * const descr_table[] =
{
  (uint8_t *)&StringLangID,
  (uint8_t *)&StringVendor,
  (uint8_t *)&StringProduct,
  (uint8_t *)&StringSerial,
};

using namespace USB_DESCRIPTORS;

//==============================================================================
// MSD Device Descriptor
//==============================================================================
constexpr DEVICE_DESCRIPTOR
< bcdUSB<0x02'00>,       // версия usb 2.0
  bDeviceClass<0>,       // Class is specified in the interface descriptor
  bDeviceSubClass<0>,    // Subclass is specified in the interface descriptor
  bDeviceProtocol<0>,    // Protocol is specified in the interface descriptor
  bMaxPacketSize0<64>,
  idVendor<0x0483>,      // VID
  idProduct<0x572C>,     // PID
  bcdDevice<0x0200>,
  iManufacturer<1>,      // индекс строки с названием производителя
  iProduct<2>,           // индекс строки с названием устройства
  iSerialNumber<3>,      // индекс строки с серийным номером устройства
  bNumConfigurations<1>  // количество поддерживаемых конфигураций
> Device_Descriptor;

//==============================================================================
// MSD Device Qualifier Descriptor
//==============================================================================
constexpr DEVICE_QUALIFIER_DESCRIPTOR
< bcdUSB<0x02'00>,       // версия usb 2.0
  bDeviceClass<0>,       // Class is specified in the interface descriptor
  bDeviceSubClass<0>,    // Subclass is specified in the interface descriptor
  bDeviceProtocol<0>,    // Protocol is specified in the interface descriptor
  bMaxPacketSize0<64>,
  bNumConfigurations<1>
> Device_Qualifier_Descriptor;


//==============================================================================
// MSD Configuration Descriptor
//==============================================================================
constexpr MSD_CONFIGURATION_DESCRIPTOR
< CONFIGURATION<
    bConfigurationValue<1>,               // Configuration 1
    iConfiguration<0>,                    // No String Descriptor
    bmAttributes<cfg_Attr::SelfPowered>,  // Self powered
    bMaxPower<100/2> >,                   // 100 mA
  
  INTERFACE_DESCRIPTOR<       // Interface 0
    bInterfaceNumber<0>,
    bAlternateSetting<0>,
    bNumEndpoints<2>,
    bInterfaceClass<8>,        // MSC Class
    bInterfaceSubClass<6>,     // SCSI transparent
    bInterfaceProtocol<0x50>,  // BULK-ONLY transport
    iInterface<0> >,           // No String Descriptor
  
  ENDPOINT_DESCRIPTOR<    // EP1 IN Bulk EndPoint
    bEndpointAddress<1,epDIR::IN>,
    bmAttributes<epTYPE::Bulk>,
    wMaxPacketSize<64>,
    bInterval<0> >,
  
  ENDPOINT_DESCRIPTOR<    // EP1 OUT Bulk EndPoint
    bEndpointAddress<1,epDIR::OUT>,
    bmAttributes<epTYPE::Bulk>,
    wMaxPacketSize<64>,
    bInterval<0> >
> Configuration_Descriptor;
