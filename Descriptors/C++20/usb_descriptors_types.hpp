#pragma once

#include <concepts>

// Концепты типов дескрипторов
template<typename T> concept is_Descriptor = std::is_base_of_v<DESCRIPTOR_BASE, T>;
template<typename T> concept is_DescriptorList = std::is_base_of_v<DESCRIPTOR_LIST_BASE, T>;
template<typename T> concept is_DescriptorListElement = is_Descriptor<T> || is_DescriptorList<T>;
template<typename T> concept is_ConfigurationDescriptor = std::is_base_of_v<CONFIGURATION_DESCRIPTOR_BASE, T>;
template<typename T> concept is_InterfaceDescriptor = std::is_base_of_v<INTERFACE_DESCRIPTOR_BASE, T>;
template<typename T> concept is_EndpointDescriptor = std::is_base_of_v<ENDPOINT_DESCRIPTOR_BASE, T>;
template<typename T> concept is_Interface_Association = std::is_base_of_v<INTERFACE_ASSOCIATION_DESCRIPTOR_BASE, T>;
template<typename T> concept is_CustomHID = std::is_base_of_v<CUSTOM_HID_DESCRIPTOR_BASE, T>;
// Концепты для полей дескрипторов
template<typename T> concept is_bmAttributes = std::is_base_of_v<BMATTRIBUTES_BASE, T>;
template<typename T> concept is_bmAttributes_EP = is_bmAttributes<T> && std::is_same_v<typename T::sub_type, epTYPE>;
template<typename T> concept is_bmAttributes_CONFIG = is_bmAttributes<T> && std::is_same_v<typename T::sub_type, cfg_Attr>;
template<typename T> concept is_bEndpointAddress = std::is_base_of_v<ENDPOINT_ADDRES_BASE, T>;

constexpr auto copy_buf(auto rcrd, uint8_t** p)
{
  for (auto& x : rcrd.buf) *(*p)++ = x;
}

//==============================================================================
// Контейнер для дескриптора
//==============================================================================
template<DescriptorType dt, typename... RCRDS>
class DESCRIPTOR : DESCRIPTOR_BASE
{
  static constexpr auto sz = (sizeof(RCRDS::buf)+...)+2;
public:
  constexpr DESCRIPTOR()
  {
    buf[0] = sz;
    buf[1] = (uint8_t)dt;
    uint8_t *p = buf+2;
    (copy_buf(RCRDS{}, &p),...);
  }
  uint8_t buf[sz]{};
};

//==============================================================================
// Контейнер для списка дескрипторов
//==============================================================================
template <is_DescriptorListElement... DSCS>
class DESCRIPTOR_LIST : DESCRIPTOR_LIST_BASE
{
  static constexpr auto ExpandDL = [](auto x)
  {
  	using T = TypeUnBox<x>;
    static_assert(is_DescriptorListElement<T>, "Only DESCRIPTOR or DESCRIPTOR_LIST");
    if constexpr (is_DescriptorList<T>) return T::GetDescriptors();
    else return TypeList<T>{}; // is_Descriptor<T>
  };
  static constexpr auto dscs_ = TypeList<DSCS...>::accumulate(ExpandDL);
public:
  constexpr DESCRIPTOR_LIST()
  {
      uint8_t* p = buf;
      (copy_buf(DSCS{}, &p), ...);
  }
  static constexpr auto GetDescriptors() { return dscs_; }
  static constexpr auto EndpointCount()
  {
  	return dscs_.filter([](auto x) { return is_EndpointDescriptor<TypeUnBox<x>>; }).size();
  }

  uint8_t buf[(sizeof(DSCS::buf) + ...)]{};
};

//==============================================================================
// Device Descriptor Type
//==============================================================================
template<is_bcdUSB TbcdUSB,                     // версия usb
         is_bDeviceClass TbDeviceClass,         // класс устройства
         is_bDeviceSubClass TbDeviceSubClass,   // подкласс
         is_bDeviceProtocol TbDeviceProtocol,   // протокол
         is_bMaxPacketSize0 TbMaxPacketSize0,   // максимальный размер пакета для нулевой конечной точки
         is_idVendor TidVendor,                 // VID
         is_idProduct TidProduct,
         is_bcdDevice TbcdDevice,
         is_iManufacturer TiManufacturer,
         is_iProduct TiProduct,
         is_iSerialNumber TiSerialNumber,
         is_bNumConfigurations TbNumConfigurations>
class DEVICE_DESCRIPTOR : public DESCRIPTOR< DescriptorType::DEVICE,
  TbcdUSB, TbDeviceClass, TbDeviceSubClass,TbDeviceProtocol,
  TbMaxPacketSize0, TidVendor, TidProduct, TbcdDevice, TiManufacturer,
  TiProduct, TiSerialNumber, TbNumConfigurations > 
{
  static constexpr auto ep0sz = TbMaxPacketSize0{}.buf[0];
  static_assert((ep0sz == 8) || (ep0sz == 16) || (ep0sz == 32) || (ep0sz == 64), "Wrong TbMaxPacketSize0 size");
};

//==============================================================================
// Device Qualifier Descriptor Type
//==============================================================================
template<is_bcdUSB TbcdUSB,
         is_bDeviceClass TbDeviceClass,
         is_bDeviceSubClass TbDeviceSubClass,
         is_bDeviceProtocol TbDeviceProtocol,
         is_bMaxPacketSize0 TbMaxPacketSize0,
         is_bNumConfigurations TbNumConfigurations>
struct DEVICE_QUALIFIER_DESCRIPTOR : public DESCRIPTOR<DescriptorType::DEVICE_QUALIFIER,
  TbcdUSB, TbDeviceClass, TbDeviceSubClass, TbDeviceProtocol, TbMaxPacketSize0,
  TbNumConfigurations, bReserved<0>> { };

//==============================================================================
// Configuration Descriptor Type
//==============================================================================
template<auto... args> class bmAttributes : BMATTRIBUTES_BASE
{
    static_assert((sizeof...(args) > 0) && (sizeof...(args) < 4), "Wrong parameters");

    template<auto arg1> static constexpr auto Chek1Args()
    {
        static_assert(std::is_same_v<decltype(arg1), cfg_Attr> || std::is_same_v<decltype(arg1), epTYPE>, "Wrong arguments");
        return  arg1;
    }
    template<auto arg1, auto arg2> static constexpr auto Chek2Args()
    {
        static_assert(std::is_same_v<decltype(arg1), epTYPE>, "Wrong arguments");
        static_assert(std::is_same_v<decltype(arg2), epSYNC> || std::is_same_v<decltype(arg2), epUSAGE>, "Wrong arguments");
        return  arg1;
    }
    template<auto arg1, auto arg2, auto arg3> static constexpr auto Chek3Args()
    {
        static_assert(std::is_same_v<decltype(arg1), epTYPE>, "Wrong arguments");
        static_assert((std::is_same_v<decltype(arg2), epSYNC> && std::is_same_v<decltype(arg3), epUSAGE>) ||
            (std::is_same_v<decltype(arg3), epSYNC> && std::is_same_v<decltype(arg2), epUSAGE>), "Wrong arguments");
        return  arg1;
    }
    static constexpr auto CheckArgs()
    {
        if constexpr (sizeof...(args) == 1) return Chek1Args<args...>();
        else if constexpr (sizeof...(args) == 2) return Chek2Args<args...>();
        else if constexpr (sizeof...(args) == 3) return Chek3Args<args...>();
        else return;
    }
public:
    using type = ValueBox<REC_TYPE::bmAttributes>;
    using sub_type = decltype(CheckArgs());
    constexpr bmAttributes()
    {
        buf[0] = (uint8_t(args) | ...);
    }
    uint8_t buf[1]{};
};

template<is_wTotalLength TwTotalLength,
         is_bNumInterfaces TbNumInterfaces,
         is_bConfigurationValue TbConfigurationValue,
         is_iConfiguration TiConfiguration,
         is_bmAttributes_CONFIG TbmAttributes,
         is_bMaxPower TbMaxPower>
struct CONFIGURATION_DESCRIPTOR : public DESCRIPTOR<DescriptorType::CONFIGURATION,
    TwTotalLength, TbNumInterfaces, TbConfigurationValue, TiConfiguration,
    TbmAttributes, TbMaxPower>, CONFIGURATION_DESCRIPTOR_BASE
{
  using bNumInterfaces = TbNumInterfaces;
  using bConfigurationValue = TbConfigurationValue;
  using iConfiguration = TiConfiguration;
  using bmAttributes = TbmAttributes;
  using bMaxPower = TbMaxPower;
};

//==============================================================================
// Device Configuration Descriptor Type
//==============================================================================
template<is_bConfigurationValue TbConfigurationValue,
         is_iConfiguration TiConfiguration,
         is_bmAttributes TbmAttributes,
         is_bMaxPower TbMaxPower,
         is_DescriptorListElement... DSCS>
class DEVICE_CONFIGURATION_DESCRIPTOR
{
  static constexpr auto sz = (sizeof(DSCS::buf)+...+9);

  static constexpr auto CalcInterfacesNum()
  {
    return DESCRIPTOR_LIST<DSCS...>::GetDescriptors().filter(
           [](auto x) { return is_InterfaceDescriptor<TypeUnBox<x>>; }).size();
  }

  using CFG_DESCR = CONFIGURATION_DESCRIPTOR<wTotalLength<sz>,
                                             bNumInterfaces<CalcInterfacesNum()>,
                                             TbConfigurationValue,
                                             TiConfiguration,
                                             TbmAttributes,
                                             TbMaxPower>;
public:
  constexpr DEVICE_CONFIGURATION_DESCRIPTOR()
  {
    uint8_t* p = buf;
    copy_buf(CFG_DESCR{}, &p);
    (copy_buf(DSCS{}, &p), ...);
  }
  uint8_t buf[sz]{};
};

//==============================================================================
// Interface Descriptor Type
//==============================================================================
template<is_bInterfaceNumber TbInterfaceNumber,
         is_bAlternateSetting TbAlternateSetting,
         is_bNumEndpoints TbNumEndpoints,
         is_bInterfaceClass TbInterfaceClass,
         is_bInterfaceSubClass TbInterfaceSubClass,
         is_bInterfaceProtocol TbInterfaceProtocol,
         is_iInterface TiInterface>
struct INTERFACE_DESCRIPTOR : public DESCRIPTOR<DescriptorType::INTERFACE,
    TbInterfaceNumber, TbAlternateSetting, TbNumEndpoints, TbInterfaceClass,
    TbInterfaceSubClass, TbInterfaceProtocol, TiInterface>, INTERFACE_DESCRIPTOR_BASE { };

//==============================================================================
// Endpoint Descriptor Type
//==============================================================================
template <uint8_t number, epDIR dir>
class bEndpointAddress : ENDPOINT_ADDRES_BASE
{
    static_assert(number < 16, "Endpoint number: Bit 3...0");
    static constexpr uint8_t value = (uint8_t)dir + number;
public:
    using type = ValueBox<REC_TYPE::bEndpointAddress>;
    uint8_t buf[1]{ value };
};

template<is_bEndpointAddress TbEndpointAddress,
         is_bmAttributes_EP TbmAttributes,
         is_wMaxPacketSize TwMaxPacketSize,
         is_bInterval TbInterval>
struct ENDPOINT_DESCRIPTOR : public DESCRIPTOR<DescriptorType::ENDPOINT,
  TbEndpointAddress, TbmAttributes, TwMaxPacketSize, TbInterval>, ENDPOINT_DESCRIPTOR_BASE { };

//==============================================================================
// Interface Association Descriptor Type
//==============================================================================
template<is_bFirstInterface TbFirstInterface,
         is_bInterfaceCount TbInterfaceCount,
         is_bFunctionClass TbFunctionClass,
         is_bFunctionSubClass TbFunctionSubClass,
         is_bFunctionProtocol TbFunctionProtocol,
         is_iFunction TiFunction>
struct INTERFACE_ASSOCIATION_DESCRIPTOR : public DESCRIPTOR<DescriptorType::INTERFACE_ASSOCIATION,
  TbFirstInterface, TbInterfaceCount, TbFunctionClass,
  TbFunctionSubClass, TbFunctionProtocol, TiFunction> { };

//==============================================================================
// CDC Header Functional Descriptor Type
//==============================================================================
template<is_bDescriptorSubType TbDescriptorSubType,
         is_bcdCDC TbcdCDC>
struct CDC_HEADER_FUNCTIONAL_DESCRIPTOR : public DESCRIPTOR<DescriptorType::CS_INTERFACE,
  TbDescriptorSubType, TbcdCDC>, CDC_HEADER_FUNCTIONAL_DESCRIPTOR_BASE { };

//==============================================================================
// CDC ACM Functional Descriptor Type
//==============================================================================
template<is_bDescriptorSubType TbDescriptorSubType,
         is_bmCapabilities TbmCapabilities>
struct CDC_ACM_FUNCTIONAL_DESCRIPTOR : public DESCRIPTOR<DescriptorType::CS_INTERFACE,
  TbDescriptorSubType, TbmCapabilities>, CDC_ACM_FUNCTIONAL_DESCRIPTOR_BASE { };

//==============================================================================
// CDC Union Functional Descriptor Type
//==============================================================================
template<is_bDescriptorSubType TbDescriptorSubType,
         is_bControlInterface TbControlInterface,
         is_bSubordinateInterface0 TbSubordinateInterface0>
struct CDC_UNION_FUNCTIONAL_DESCRIPTOR : public DESCRIPTOR<DescriptorType::CS_INTERFACE,
  TbDescriptorSubType, TbControlInterface, TbSubordinateInterface0>, CDC_UNION_FUNCTIONAL_DESCRIPTOR_BASE {};

//==============================================================================
// CDC Call Management Functional Descriptor Type
//==============================================================================
template<is_bDescriptorSubType TbDescriptorSubType,
         is_bmCapabilities TbmCapabilities,
         is_bDataInterface TbDataInterface>
struct CDC_CALL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR : public DESCRIPTOR<DescriptorType::CS_INTERFACE,
  TbDescriptorSubType, TbmCapabilities,
  TbDataInterface>, CDC_CALL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR_BASE {};

//==============================================================================
// CUSTOM HID Descriptor Type
//==============================================================================
template<is_bcdHID TbcdHID,                           // HID Version ( 1.11 )
         is_bCountryCode TbCountryCode,               // Код страны локализованного устройства
         is_bNumDescriptors TbNumDescriptors,         // Количество дескрипторов в классе
         is_bDescriptorType_0 TbDescriptorType_0,     // Тип дескриптора Report Descriptor
         is_wDescriptorLength_0 TwDescriptorLength_0> // Длина Report Descriptor
struct CUSTOM_HID_DESCRIPTOR : public DESCRIPTOR<DescriptorType::HID,
  TbcdHID, TbCountryCode, TbNumDescriptors,
  TbDescriptorType_0, TwDescriptorLength_0>, CUSTOM_HID_DESCRIPTOR_BASE {};

//==============================================================================
// Custom HID Configuration Descriptor Type
//==============================================================================
template<is_bConfigurationValue TbConfigurationValue,
         is_iConfiguration TiConfiguration,
         is_bmAttributes TbmAttributes,
         is_bMaxPower TbMaxPower,
         is_InterfaceDescriptor Interface,
         is_CustomHID CustomHID,
         is_EndpointDescriptor EP_IN,
         is_EndpointDescriptor EP_OUT>
struct CUSOM_HID_CONFIGURATION_DESCRIPTOR : DEVICE_CONFIGURATION_DESCRIPTOR<
  TbConfigurationValue, TiConfiguration, TbmAttributes, TbMaxPower,
  Interface, CustomHID, EP_IN, EP_OUT>
{
    static constexpr auto Custom_HID_Descriptor_Size() { return sizeof(CustomHID::buf); }
    static constexpr auto Custom_HID_Descriptor_Adr(uint8_t *config_descriptor)
    {
        return config_descriptor + 9 + sizeof(Interface);
    }
};

//==============================================================================
// WINUSB Configuration Descriptor Type
//==============================================================================
template<is_bConfigurationValue TbConfigurationValue,
         is_iConfiguration TiConfiguration,
         is_bmAttributes TbmAttributes,
         is_bMaxPower TbMaxPower,
         is_InterfaceDescriptor Interface,
         is_EndpointDescriptor EP_IN,
         is_EndpointDescriptor EP_OUT>
struct WINUSB_CONFIGURATION_DESCRIPTOR : DEVICE_CONFIGURATION_DESCRIPTOR<
  TbConfigurationValue, TiConfiguration, TbmAttributes, TbMaxPower,
  Interface, EP_IN, EP_OUT> { };

//==============================================================================
// MSD Configuration Descriptor Type
//==============================================================================
template<is_bConfigurationValue TbConfigurationValue,
         is_iConfiguration TiConfiguration,
         is_bmAttributes TbmAttributes,
         is_bMaxPower TbMaxPower,
         is_InterfaceDescriptor Interface,
         is_EndpointDescriptor EP_IN,
         is_EndpointDescriptor EP_OUT>
struct MSD_CONFIGURATION_DESCRIPTOR : DEVICE_CONFIGURATION_DESCRIPTOR<
  TbConfigurationValue, TiConfiguration, TbmAttributes, TbMaxPower,
  Interface, EP_IN, EP_OUT> { };
