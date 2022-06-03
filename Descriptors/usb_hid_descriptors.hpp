#pragma once

#if (__cplusplus > 201703L)
#include "C++20/usb_descriptors.hpp"
#else
#include "C++17/usb_descriptors.h"
#endif 

STRING_DESCRIPTOR(0, StringLangID,    u"\x0409"                );
STRING_DESCRIPTOR(1, StringVendor,    u"STMicroelectronics"    );
STRING_DESCRIPTOR(2, StringProduct,   u"STM32 Custom HID"      );
STRING_DESCRIPTOR(3, StringSerial,    u"00000000001B"          );

inline const uint8_t * const descr_table[] =
{
  (uint8_t *)&StringLangID,
  (uint8_t *)&StringVendor,
  (uint8_t *)&StringProduct,
  (uint8_t *)&StringSerial
};

using namespace USB_DESCRIPTORS;

//==============================================================================
// Device Descriptor
//==============================================================================
constexpr DEVICE_DESCRIPTOR
< bcdUSB<0x02'00>,       // версия usb 2.0
  bDeviceClass<0>,       // Class is specified in the interface descriptor
  bDeviceSubClass<0>,    // Subclass is specified in the interface descriptor
  bDeviceProtocol<0>,    // Protocol is specified in the interface descriptor
  bMaxPacketSize0<64>,
  idVendor<0x0483>,      // VID
  idProduct<0x572A>,     // PID
  bcdDevice<0x0200>,
  iManufacturer<1>,      // индекс строки с названием производителя
  iProduct<2>,           // индекс строки с названием устройства
  iSerialNumber<3>,      // индекс строки с серийным номером устройства
  bNumConfigurations<1>  // количество поддерживаемых конфигураций
> Device_Descriptor;

//==============================================================================
// Device Qualifier Descriptor
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
// HID Report Descriptor
//==============================================================================
using namespace HID_REPORT;

constexpr HID_REPORT_DESCRIPTOR<
  UsagePage<USAGE_PAGE::VENDOR_DEFINED_PAGE_1>,
  Usage<1>,
  COLLECTION_APPLICATION<
   ReportID<1>,
   Usage<1>,
   LogicalMinMax<0,1>,
   ReportFormat<8,1>,
   Feature<0x82>,
   ReportID<1>,
   Usage<1>,
   Output<0x82>,

   ReportID<2>,
   Usage<2>,
   LogicalMinMax<0,1>,
   ReportFormat<8,1>,
   Feature<0x82>,
   ReportID<2>,
   Usage<2>,
   Output<0x82>,

   ReportID<3>,
   Usage<3>,
   LogicalMinMax<0,255>,
   ReportFormat<8,1>,
   Feature<0x82>,
   ReportID<3>,
   Usage<3>,
   Output<0x82>,

   ReportID<4>,
   Usage<3>,
   LogicalMinMax<0,255>,
   ReportFormat<8,1>,
   Feature<0x82>,
   ReportID<4>,
   Usage<3>,
   Output<0x82>,

   ReportID<5>,
   Usage<4>,
   ReportFormat<8,1>,
   Input<0x02>
  >
> HidReportDescriptor;

//==============================================================================
// HID Configuration Descriptor
//==============================================================================
constexpr CUSOM_HID_CONFIGURATION_DESCRIPTOR
< bConfigurationValue<1>,               // configuration 1
  iConfiguration<0>,                    // No String Descriptor
  bmAttributes<cfg_Attr::SelfPowered>,  // Self powered
  bMaxPower<100/2>,                     // 100 mA
  
  INTERFACE_DESCRIPTOR <    // Interface 0 - CUSTOM HID
   bInterfaceNumber<0>,
    bAlternateSetting<0>,
    bNumEndpoints<2>,
    bInterfaceClass<3>,     // Custom HID
    bInterfaceSubClass<0>,  // 1=BOOT, 0=no boot
    bInterfaceProtocol<0>,  // 0=none, 1=keyboard, 2=mouse
    iInterface<0> >,
  
  CUSTOM_HID_DESCRIPTOR<
    bcdHID<0x01'11>,                                   // HID Version ( 1.11 )
    bCountryCode<HID_Localization::Not_Localized>,     // HID_Localization::Not_Localized,
    bNumDescriptors<1>,                                // Количество дескрипторов в классе
    bDescriptorType_0<DescriptorType::REPORT>,
    wDescriptorLength_0<sizeof(HidReportDescriptor)> >,// Длина Report Descriptor
  
  ENDPOINT_DESCRIPTOR<    // EP1 IN Interrupt EndPoint
    bEndpointAddress<1,epDIR::IN>,
    bmAttributes<epTYPE::Interrupt>,
    wMaxPacketSize<2>,
    bInterval<20> >,
  
  ENDPOINT_DESCRIPTOR<    // EP1 OUT Interrupt EndPoint
    bEndpointAddress<1,epDIR::OUT>,
    bmAttributes<epTYPE::Interrupt>,
    wMaxPacketSize<2>,
    bInterval<0> >  
> Configuration_Descriptor;
