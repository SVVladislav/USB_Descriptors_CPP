#pragma once

#if (__cplusplus > 201703L)
#include "C++20/usb_descriptors.hpp"
#else
#include "C++17/usb_descriptors.h"
#endif 

STRING_DESCRIPTOR( 0, StringLangID,    u"\x0409"                );
STRING_DESCRIPTOR( 1, StringVendor,    u"STMicroelectronics"    );
STRING_DESCRIPTOR( 2, StringProduct,   u"STM32 Virtual ComPort" );
STRING_DESCRIPTOR( 3, StringSerial,    u"00000000001A"          );

inline const uint8_t * const descr_table[] =
{
  (uint8_t *)&StringLangID,
  (uint8_t *)&StringVendor,
  (uint8_t *)&StringProduct,
  (uint8_t *)&StringSerial,
};

using namespace USB_DESCRIPTORS;

//==============================================================================
// Device Descriptor
//==============================================================================
constexpr DEVICE_DESCRIPTOR
< bcdUSB<0x02'00>,       // версия usb 2.0
  bDeviceClass<2>,       // Communications and CDC Control
  bDeviceSubClass<2>,    //
  bDeviceProtocol<0>,    // No class specific protocol required
  bMaxPacketSize0<64>,
  idVendor<0x0483>,      // VID
  idProduct<0x5740>,     // PID
  bcdDevice<0x0200>,
  iManufacturer<1>,
  iProduct<2>,
  iSerialNumber<3>,
  bNumConfigurations<1>
> Device_Descriptor;

//==============================================================================
// Device Qualifier Descriptor
//==============================================================================
constexpr DEVICE_QUALIFIER_DESCRIPTOR
< bcdUSB<0x02'00>,       // версия usb 2.0
  bDeviceClass<2>,       // Communications and CDC Control
  bDeviceSubClass<2>,    //
  bDeviceProtocol<0>,    // No class specific protocol required
  bMaxPacketSize0<64>,
  bNumConfigurations<0>
> Device_Qualifier_Descriptor;

//==============================================================================
// CDC VCP Configuration Descriptor
//==============================================================================
constexpr DEVICE_CONFIGURATION_DESCRIPTOR
<   bConfigurationValue<1>,               // configuration 1
    iConfiguration<0>,                    // No String Descriptor
    bmAttributes<cfg_Attr::SelfPowered>,  // Self powered
    bMaxPower<100/2>,                     // 100 mA

  INTERFACE_DESCRIPTOR<     // Interface 0 - CDC Communication
    bInterfaceNumber<0>,
    bAlternateSetting<0>,
    bNumEndpoints<1>,
    bInterfaceClass<2>,     // Communications and CDC Control
    bInterfaceSubClass<2>,  // Abstract Control Model
    bInterfaceProtocol<1>,  // AT Commands defined by ITU-T V.250 etc
    iInterface<0> >,

  CDC_HEADER_FUNCTIONAL_DESCRIPTOR<
    bDescriptorSubType<0>,    // Header Functional Descriptor
    bcdCDC<0x01'10> >,        // CDC Version 1.10

  CDC_ACM_FUNCTIONAL_DESCRIPTOR<
    bDescriptorSubType<2>, // Abstract Control Management Functional Descriptor
    bmCapabilities<2> >,   //

  CDC_UNION_FUNCTIONAL_DESCRIPTOR<
    bDescriptorSubType<6>,       // Union Functional Descriptor
    bControlInterface<0>,        // Interface 0  -
    bSubordinateInterface0<1> >, // Interface 1  -  Data Class Interface

  CDC_CALL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR<
    bDescriptorSubType<1>,     // Call Management Functional Descriptor
    bmCapabilities<0>,
    bDataInterface<1> >,       //

  ENDPOINT_DESCRIPTOR<      // EP2 In Interrupt EndPoint
    bEndpointAddress<2, epDIR::IN>,
    bmAttributes<epTYPE::Interrupt>,
    wMaxPacketSize<8>,
    bInterval<255> >,

  INTERFACE_DESCRIPTOR<     // Interface 1 - Data Interface
    bInterfaceNumber<1>,
    bAlternateSetting<0>,
    bNumEndpoints<2>,
    bInterfaceClass<0x0A>, // CDC-Data
    bInterfaceSubClass<0>,
    bInterfaceProtocol<0>,
    iInterface<0> >,

  ENDPOINT_DESCRIPTOR<    // EP1 OUT Bulk EndPoint
    bEndpointAddress<1, epDIR::OUT>,
    bmAttributes<epTYPE::Bulk>,
    wMaxPacketSize<64>,
    bInterval<0> >,

  ENDPOINT_DESCRIPTOR<    // EP1 IN Bulk EndPoint
    bEndpointAddress<1, epDIR::IN>,
    bmAttributes<epTYPE::Bulk>,
    wMaxPacketSize<64>,
    bInterval<0> >

> Configuration_Descriptor;



