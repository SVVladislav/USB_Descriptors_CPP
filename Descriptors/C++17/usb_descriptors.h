#pragma once

#include <stdint.h>
#include <algorithm>
#include "TypeList.h"

enum class DescriptorType : uint8_t
{
  DEVICE=1, CONFIGURATION=2, STRING=3, INTERFACE=4, ENDPOINT=5,
  DEVICE_QUALIFIER=6, OTHER_SPEED_CONFIGURATION=7, INTERFACE_POWER=8,
  INTERFACE_ASSOCIATION=0xB,
  HID=0x21, REPORT=0x22, PHYSICAL=0x23, // HID v1.11 spec. Chapter 7.1
  CS_INTERFACE=0x24, CS_ENDPOINT=0x25   // Class Specified
};

enum class HID_Localization : uint8_t
{
  Not_Localized=0, Arabic=1, Belgian=2, Canadian_Bilingual=3, Canadian_French=4,
  Czech_Republic=5, Danish=6, Finnish=7, French=8, German=9, Greek=10, Hebrew=11,
  Hungary=12, International=13, Italian=14, Japan=15, Korean=16, Latin_American=17,
  Netherlands_Dutch=18, Norwegian=19, Persian=20, Poland=21, Portuguese=22,
  Russia=23, Slovakia=24, Spanish=25, Swedish=26, Swiss_French=27, Swiss_German=28,
  Switzerland=29, Taiwan=30, Turkish_Q=31, UK=32, US=33, Yugoslavia=34, Turkish_F=35
};

//==============================================================================
// String Descriptors
//==============================================================================
#define STRING_DESCRIPTOR(idx, name, text)                   \
inline constexpr struct __attribute__((__packed__))          \
{                                                            \
  uint8_t bIndex;                                            \
  uint8_t bLength;                                           \
  DescriptorType bDescriptorType;                            \
  char16_t Text[std::size(text)];                            \
} name = { idx, sizeof(text), DescriptorType::STRING, text };


namespace USB_DESCRIPTORS
{

// Типизация полей дескрипторов
enum class REC_TYPE 
{ 
  // DEVICE_DESCRIPTOR
  bLength, bDescriptorType, bcdUSB, bDeviceClass, bDeviceSubClass, bDeviceProtocol,
  bMaxPacketSize0, idVendor, idProduct, bcdDevice, iManufacturer, iProduct, iSerialNumber,
  bNumConfigurations, bReserved, 
  // CONFIGURATION_DESCRIPTOR_
  wTotalLength, bNumInterfaces, bConfigurationValue,
  iConfiguration, bmAttributes, bMaxPower, 
  // INTERFACE_DESCRIPTOR                      
  bInterfaceNumber, bAlternateSetting, bNumEndpoints, bInterfaceClass, 
  bInterfaceSubClass, bInterfaceProtocol, iInterface,
  // ENDPOINT_DESCRIPTOR
  bEndpointAddress, wMaxPacketSize, bInterval,
  // INTERFACE_ASSOCIATION_DESCRIPTOR
  bFirstInterface, bInterfaceCount, bFunctionClass, 
  bFunctionSubClass, bFunctionProtocol, iFunction,
  // CDC_HEADER_FUNCTIONAL_DESCRIPTOR
  bDescriptorSubType, bcdCDC,
  // CDC_ACM_FUNCTIONAL_DESCRIPTOR
  bmCapabilities,
  // CDC_UNION_FUNCTIONAL_DESCRIPTOR
  bControlInterface, bSubordinateInterface0,
  // CDC_CALL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR
  bDataInterface, 
  // CUSTOM_HID_DESCRIPTOR_BASE
  bcdHID, bCountryCode, bNumDescriptors, bDescriptorType_0, wDescriptorLength_0 
};
  
// Базовые классы для дескрипторов
class DESCRIPTOR_BASE {};
class DESCRIPTOR_LIST_BASE {};
class CONFIGURATION_DESCRIPTOR_BASE {};
class CONFIG_DESCRIPTOR_BASE {};
class INTERFACE_DESCRIPTOR_BASE {};
class ENDPOINT_DESCRIPTOR_BASE {};
class INTERFACE_ASSOCIATION_DESCRIPTOR_BASE {};
class CDC_HEADER_FUNCTIONAL_DESCRIPTOR_BASE {};
class CDC_ACM_FUNCTIONAL_DESCRIPTOR_BASE {};
class CDC_UNION_FUNCTIONAL_DESCRIPTOR_BASE {};
class CDC_CALL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR_BASE {};
class CUSTOM_HID_DESCRIPTOR_BASE {};
class BMATTRIBUTES_BASE {};
class ENDPOINT_ADDRES_BASE {};

//==============================================================================
// Определение полей дескрипторов
//==============================================================================
template<typename T, REC_TYPE rt> constexpr bool IsRecType() { return value_unbox<T>()==rt; }    

#define REC_U8(X) template<typename U> constexpr bool is_##X() { return IsRecType<U,REC_TYPE::X>(); } \
                  template<uint8_t x> using X = HOLDER<REC_TYPE::X,(uint8_t)x>                   

#define REC_U16(X) template<typename T> constexpr bool is_##X() { return IsRecType<T,REC_TYPE::X>(); } \
                   template<uint16_t x> using X = HOLDER<REC_TYPE::X,uint8_t(x),uint8_t(x>>8)>

#define REC_8(T,X) template<typename U> constexpr bool is_##X() { return IsRecType<U,REC_TYPE::X>(); } \
                   template<T x> using X = HOLDER<REC_TYPE::X,(uint8_t)x>

// Контейнер для полей дескрипторов
template <REC_TYPE rt, uint8_t... data>
struct HOLDER
{
    using type = ValueBox<rt>;
    uint8_t buf[sizeof...(data)]{ data... };
};

// Device Descriptor records
REC_U8(bLength);
REC_U8(bDescriptorType);
REC_U16(bcdUSB);
REC_U8(bDeviceClass);
REC_U8(bDeviceSubClass);
REC_U8(bDeviceProtocol);
REC_U8(bMaxPacketSize0);
REC_U16(idVendor);
REC_U16(idProduct);
REC_U16(bcdDevice);
REC_U8(iManufacturer);
REC_U8(iProduct);
REC_U8(iSerialNumber);
REC_U8(bNumConfigurations);
REC_U8(bReserved);
// Configuration Descriptor records
REC_U16(wTotalLength);
REC_U8(bNumInterfaces);
REC_U8(bConfigurationValue);
REC_U8(iConfiguration);
REC_U8(bMaxPower);
// bmAttributes
enum class cfg_Attr : uint8_t { SelfPowered = 0xC0, RemoteWakeup = 0xA0 };
enum class epTYPE : uint8_t { Control = 0, Isochronous = 1, Bulk = 2, Interrupt = 3 };
enum class epSYNC : uint8_t { NoSynchronization = 0, Asynchronous = 4, Adaptive = 6, Synchronous = 7 };
enum class epUSAGE : uint8_t { Data = 0, Feedback = 0x10, ImplicitFeedbackData = 0x20 };
// Interface Descriptor records
REC_U8(bInterfaceNumber);
REC_U8(bAlternateSetting);
REC_U8(bNumEndpoints);
REC_U8(bInterfaceClass);
REC_U8(bInterfaceSubClass);
REC_U8(bInterfaceProtocol);
REC_U8(iInterface);
// Endpoint Descriptor records
REC_U16(wMaxPacketSize);
REC_U8(bInterval);
// bEndpointAddress
enum class epDIR : uint8_t { OUT = 0, IN = 0x80 };
// Interface Association Descriptor 
REC_U8(bFirstInterface);
REC_U8(bInterfaceCount);
REC_U8(bFunctionClass);
REC_U8(bFunctionSubClass);
REC_U8(bFunctionProtocol);
REC_U8(iFunction);
// CDC Header Functional Descriptor records
REC_U8(bDescriptorSubType);
REC_U16(bcdCDC);
// CDC ACM Functional Descriptor records
REC_U8(bmCapabilities);
// CDC Union Functional Descriptor records
REC_U8(bControlInterface);
REC_U8(bSubordinateInterface0);
// CDC Call Management Functional Descriptor records
REC_U8(bDataInterface);
// CUSTOM_HID Descriptor records
REC_U16(bcdHID);
REC_8(HID_Localization, bCountryCode);
REC_U8(bNumDescriptors);
REC_8(DescriptorType, bDescriptorType_0);
REC_U16(wDescriptorLength_0);

#include "usb_descriptors_types.h"
#include "usb_hid_report_descriptors_types.h"

//==============================================================================
// WINUSB Compatible ID Feature Descriptor Type
//==============================================================================
struct __attribute__((__packed__)) WINUSB_COMPATIBLE_ID_FEATURE_DESCRIPTOR
{
  uint32_t  dwLength;
  uint16_t  bcdVersion;
  uint16_t  wCompatibilityID;
  uint8_t   bSections;
  uint8_t   bReserv1[7];
  uint8_t   bInterface;
  uint8_t   bReserv2;
  char      IDString[8];
  uint8_t   bReserv3[14];
};

} // namespace USB_DESCRIPTOR
