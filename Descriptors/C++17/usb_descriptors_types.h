#pragma once

// "Концепты" типов дескрипторов
template<typename T> constexpr bool is_Descriptor() { return std::is_base_of_v<DESCRIPTOR_BASE, T>; }
template<typename T> constexpr bool is_DescriptorList() { return std::is_base_of_v<DESCRIPTOR_LIST_BASE, T>; }
template<typename T> constexpr bool is_DescriptorListElement() { return is_Descriptor<T>() || is_DescriptorList<T>(); }
template<typename T> constexpr bool is_ConfigurationDescriptor() { return std::is_base_of_v<CONFIGURATION_DESCRIPTOR_BASE,T>; }
template<typename T> constexpr bool is_InterfaceDescriptor() { return std::is_base_of_v<INTERFACE_DESCRIPTOR_BASE,T>; }
template<typename T> constexpr bool is_EndpointDescriptor() { return std::is_base_of_v<ENDPOINT_DESCRIPTOR_BASE,T>; }
template<typename T> constexpr bool is_Interface() { return std::is_base_of_v<INTERFACE_BASE, T>; }
// "Концепты" для полей дескрипторов
template<typename T> constexpr bool is_bmAttributes() { return std::is_base_of_v<BMATTRIBUTES_BASE,T>; }
template<typename T> constexpr bool is_bmAttributes_EP()
{
  return is_bmAttributes<T>() && std::is_same_v<typename T::sub_type, epTYPE>;
}
template<typename T> constexpr bool is_bmAttributes_CONFIG()
{
  return is_bmAttributes<T>() && std::is_same_v<typename T::sub_type, cfg_Attr>;
}
template<typename RCRD>
constexpr auto copy_buf(RCRD rcrd, uint8_t **p)
{
  for(auto &x : rcrd.buf) *(*p)++ = x;
}
template<typename T> constexpr bool is_Interface_Association()
{
  return std::is_base_of_v<INTERFACE_ASSOCIATION_DESCRIPTOR_BASE,T>;
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
template <typename... DSCS>
class DESCRIPTOR_LIST : DESCRIPTOR_LIST_BASE
{
  static_assert((is_DescriptorListElement<DSCS>()&&...),
                "DSCS is't Descriptor List Element");

  static constexpr auto ExpandDL = [](auto x)
                                   { using T = type_unbox<decltype(x)>;
                                     if constexpr (is_DescriptorList<T>()) return T::GetDescriptors();
                                     else return TypeList<T>{}; // is_Descriptor<T>
                                   };
  static constexpr auto dscs_ = TypeList<DSCS...>::accumulate(ExpandDL);
public:
  constexpr DESCRIPTOR_LIST()
  {
    uint8_t *p = buf;
    (copy_buf(DSCS{}, &p),...);
  }
  static constexpr auto GetDescriptors() { return dscs_; }
  static constexpr uint8_t EndpointsCount()
  {
    return dscs_.filter([](auto x) { return is_EndpointDescriptor<type_unbox<decltype(x)>>(); }).size();
  }
  static constexpr uint8_t InterfacesCount()
  {
    return dscs_.filter([](auto x) { return is_InterfaceDescriptor<type_unbox<decltype(x)>>(); }).size();
  }
  uint8_t buf[(sizeof(DSCS::buf)+...)]{};
};

//==============================================================================
// Device Descriptor Type
//==============================================================================
template<typename TbcdUSB,
         typename TbDeviceClass,
         typename TbDeviceSubClass,
         typename TbDeviceProtocol,
         typename TbMaxPacketSize0,
         typename TidVendor,
         typename TidProduct,
         typename TbcdDevice,
         typename TiManufacturer,
         typename TiProduct,
         typename TiSerialNumber,
         typename TbNumConfigurations>
class DEVICE_DESCRIPTOR : public DESCRIPTOR<DescriptorType::DEVICE,
  TbcdUSB, TbDeviceClass, TbDeviceSubClass, TbDeviceProtocol, TbMaxPacketSize0,
  TidVendor, TidProduct, TbcdDevice, TiManufacturer, TiProduct,
  TiSerialNumber, TbNumConfigurations>
{
  static_assert(is_bcdUSB<TbcdUSB>(),                   "Not bcdUSB record");
  static_assert(is_bDeviceClass<TbDeviceClass>(),       "Not bDeviceClass record");
  static_assert(is_bDeviceSubClass<TbDeviceSubClass>(), "Not bDeviceSubClass record");
  static_assert(is_bDeviceProtocol<TbDeviceProtocol>(), "Not bDeviceProtocol record");
  static_assert(is_bMaxPacketSize0<TbMaxPacketSize0>(), "Not bMaxPacketSize0 record");
  static_assert(is_idVendor<TidVendor>(),               "Not idVendor record");
  static_assert(is_idProduct<TidProduct>(),             "Not idProduct record");
  static_assert(is_bcdDevice<TbcdDevice>(),             "Not bcdDevice record");
  static_assert(is_iManufacturer<TiManufacturer>(),     "Not iManufacturer record");
  static_assert(is_iProduct<TiProduct>(),               "Not iProduct record");
  static_assert(is_iSerialNumber<TiSerialNumber>(),     "Not iSerialNumber record");
  static_assert(is_bNumConfigurations<TbNumConfigurations>(), "Not bNumConfigurations record");
  static constexpr auto ep0sz =  TbMaxPacketSize0{}.buf[0];    
  static_assert( (ep0sz==8)||(ep0sz==16)||(ep0sz==32)||(ep0sz==64),"Wrong TbMaxPacketSize0 size");
  
};

//==============================================================================
// Device Qualifier Descriptor Type
//==============================================================================
template<typename TbcdUSB,
         typename TbDeviceClass,
         typename TbDeviceSubClass,
         typename TbDeviceProtocol,
         typename TbMaxPacketSize0,
         typename TbNumConfigurations>
struct DEVICE_QUALIFIER_DESCRIPTOR : public DESCRIPTOR<DescriptorType::DEVICE_QUALIFIER,
  TbcdUSB, TbDeviceClass, TbDeviceSubClass, TbDeviceProtocol, TbMaxPacketSize0,
  TbNumConfigurations, bReserved<0>>
{
  static_assert(is_bcdUSB<TbcdUSB>(),                   "Not bcdUSB record");
  static_assert(is_bDeviceClass<TbDeviceClass>(),       "Not bDeviceClass record");
  static_assert(is_bDeviceSubClass<TbDeviceSubClass>(), "Not bDeviceSubClass record");
  static_assert(is_bDeviceProtocol<TbDeviceProtocol>(), "Not bDeviceProtocol record");
  static_assert(is_bMaxPacketSize0<TbMaxPacketSize0>(), "Not bMaxPacketSize0 record");
  static_assert(is_bNumConfigurations<TbNumConfigurations>(), "Not bNumConfigurations record");
};

//==============================================================================
// Configuration Descriptor Types
//==============================================================================
template<auto... args> class bmAttributes : BMATTRIBUTES_BASE
{
  static_assert((sizeof...(args)>0)&&(sizeof...(args)<4),"Wrong bmAttributes arguments");

  template<auto arg1> static constexpr auto Chek1Args()
  {
    static_assert(std::is_same_v<decltype(arg1),cfg_Attr> || std::is_same_v<decltype(arg1),epTYPE>, "Wrong bmAttributes arguments");
    return  arg1;
  }
  template<auto arg1, auto arg2> static constexpr auto Chek2Args()
  {
    static_assert( std::is_same_v<decltype(arg1),epTYPE>, "Wrong bmAttributes arguments");
    static_assert( std::is_same_v<decltype(arg2),epSYNC> || std::is_same_v<decltype(arg2),epUSAGE>, "Wrong bmAttributes arguments");
    return  arg1;
  }
  template<auto arg1, auto arg2, auto arg3> static constexpr auto Chek3Args()
  {
    static_assert( std::is_same_v<decltype(arg1),epTYPE>, "Wrong bmAttributes arguments");
    static_assert( (std::is_same_v<decltype(arg2),epSYNC> && std::is_same_v<decltype(arg3),epUSAGE>) ||
                   (std::is_same_v<decltype(arg3),epSYNC> && std::is_same_v<decltype(arg2),epUSAGE>), "Wrong bmAttributes arguments");
    return  arg1;
  }
  static constexpr auto CheckArgs()
  {
    if constexpr (sizeof...(args)==1) return Chek1Args<args...>();
    else if constexpr (sizeof...(args)==2) return Chek2Args<args...>();
    else if constexpr (sizeof...(args)==3) return Chek3Args<args...>();
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

template<typename TwTotalLength,
         typename TbNumInterfaces,
         typename TbConfigurationValue,
         typename TiConfiguration,
         typename TbmAttributes,
         typename TbMaxPower>
struct CONFIGURATION_DESCRIPTOR : public DESCRIPTOR<DescriptorType::CONFIGURATION,
  TwTotalLength, TbNumInterfaces, TbConfigurationValue, TiConfiguration,
  TbmAttributes, TbMaxPower>, CONFIGURATION_DESCRIPTOR_BASE
{
  using bNumInterfaces = TbNumInterfaces;
  using bConfigurationValue = TbConfigurationValue;
  using iConfiguration = TiConfiguration;
  using bmAttributes = TbmAttributes;
  using bMaxPower = TbMaxPower;

  static_assert(is_wTotalLength<TwTotalLength>(),        "Not wTotalLength record");
  static_assert(is_bNumInterfaces<TbNumInterfaces>(),    "Not bNumInterfaces record");
  static_assert(is_bConfigurationValue<TbConfigurationValue>(), "Not bConfigurationValue record");
  static_assert(is_iConfiguration<TiConfiguration>(),    "Not iConfiguration record");
  static_assert(is_bmAttributes_CONFIG<TbmAttributes>(), "Not Configuration bmAttributes record");
  static_assert(is_bMaxPower<TbMaxPower>(),              "Not bMaxPower record");
};
  
//==============================================================================
// Device Configuration Descriptor Type
//==============================================================================
template<typename TbConfigurationValue,
         typename TiConfiguration,
         typename TbmAttributes,
         typename TbMaxPower,
         typename...DSCS>
class DEVICE_CONFIGURATION_DESCRIPTOR
{
  static_assert(is_bConfigurationValue<TbConfigurationValue>(), "Not bConfigurationValue record");
  static_assert(is_iConfiguration<TiConfiguration>(),    "Not iConfiguration record");
  static_assert(is_bmAttributes_CONFIG<TbmAttributes>(), "Not Configuration bmAttributes record");
  static_assert(is_bMaxPower<TbMaxPower>(),              "Not bMaxPower record");
  static_assert((is_DescriptorListElement<DSCS>()&&...), "DSCS not Descriptor List Element");

  static constexpr auto sz = (sizeof(DSCS::buf)+...+9);

  using CFG_DESCR = CONFIGURATION_DESCRIPTOR<wTotalLength<sz>,
                                             bNumInterfaces<DESCRIPTOR_LIST<DSCS...>{}.InterfacesCount()>,
                                             TbConfigurationValue,
                                             TiConfiguration,
                                             TbmAttributes,
                                             TbMaxPower>;
public:
  constexpr DEVICE_CONFIGURATION_DESCRIPTOR()
  {
    uint8_t *p = buf;
    copy_buf(CFG_DESCR{}, &p);
    (copy_buf(DSCS{}, &p),...);
  }
  uint8_t buf[sz]{};
};  
  
//==============================================================================
// Interface Descriptor Type
//==============================================================================
template<typename TbInterfaceNumber,
         typename TbAlternateSetting,
         typename TbNumEndpoints,
         typename TbInterfaceClass,
         typename TbInterfaceSubClass,
         typename TbInterfaceProtocol,
         typename TiInterface>
struct INTERFACE_DESCRIPTOR : public DESCRIPTOR<DescriptorType::INTERFACE,
  TbInterfaceNumber, TbAlternateSetting, TbNumEndpoints, TbInterfaceClass,
  TbInterfaceSubClass, TbInterfaceProtocol, TiInterface>, INTERFACE_DESCRIPTOR_BASE
{
  static_assert(is_bInterfaceNumber<TbInterfaceNumber>(),    "Not bInterfaceNumber record");
  static_assert(is_bAlternateSetting<TbAlternateSetting>(),  "Not bAlternateSetting record");
  static_assert(is_bNumEndpoints<TbNumEndpoints>(),          "Not bNumEndpoints record");
  static_assert(is_bInterfaceClass<TbInterfaceClass>(),      "Not bInterfaceClass record");
  static_assert(is_bInterfaceSubClass<TbInterfaceSubClass>(),"Not bInterfaceSubClass record");
  static_assert(is_bInterfaceProtocol<TbInterfaceProtocol>(),"Not bInterfaceProtocol record");
  static_assert(is_iInterface<TiInterface>(),                "Not iInterface record");
};

//==============================================================================
// Interface Type
//==============================================================================
template<typename TbInterfaceNumber,
         typename TbAlternateSetting,
         typename TbInterfaceClass,
         typename TbInterfaceSubClass,
         typename TbInterfaceProtocol,
         typename TiInterface,
         typename...DSCS>
class INTERFACE : public DESCRIPTOR_LIST<
   INTERFACE_DESCRIPTOR<TbInterfaceNumber, TbAlternateSetting, bNumEndpoints<0>, 
                        TbInterfaceClass, TbInterfaceSubClass, TbInterfaceProtocol, 
                        TiInterface>, DSCS...>, INTERFACE_BASE
{
  static_assert((is_DescriptorListElement<DSCS>()&&...), "DSCS not Descriptor List Element");

  static constexpr auto sz = (sizeof(DSCS::buf)+...+9);
  
  using IF_DESCR = INTERFACE_DESCRIPTOR<TbInterfaceNumber,
                                        TbAlternateSetting,
                                        bNumEndpoints<DESCRIPTOR_LIST<DSCS...>{}.EndpointsCount()>,
                                        TbInterfaceClass,
                                        TbInterfaceSubClass,
                                        TbInterfaceProtocol,
                                        TiInterface>;
public:  
  constexpr INTERFACE()
  {
    uint8_t *p = buf;
    copy_buf(IF_DESCR{}, &p);
    (copy_buf(DSCS{}, &p),...);
  }
  uint8_t buf[sz]{};
};

//==============================================================================
// Endpoint Descriptor Type
//==============================================================================
template <uint8_t number, epDIR dir>
class bEndpointAddress : ENDPOINT_ADDRES_BASE
{
  static_assert(number<16, "Endpoint number: Bit 3...0");
  static constexpr uint8_t value = (uint8_t)dir + number;
public:
  using type = ValueBox<REC_TYPE::bEndpointAddress>;
  uint8_t buf[1]{value};
};
template<typename T> constexpr bool is_bEndpointAddress() { return std::is_base_of_v<ENDPOINT_ADDRES_BASE,T>; }

template<typename TbEndpointAddress,
         typename TbmAttributes,
         typename TwMaxPacketSize,
         typename TbInterval>
struct ENDPOINT_DESCRIPTOR : public DESCRIPTOR<DescriptorType::ENDPOINT,
  TbEndpointAddress, TbmAttributes, TwMaxPacketSize, TbInterval>, ENDPOINT_DESCRIPTOR_BASE
{
  static_assert(is_bEndpointAddress<TbEndpointAddress>(), "Not bInterfaceNumber record");
  static_assert(is_bmAttributes_EP<TbmAttributes>(), "Not Endpoint bInterfaceNumber record");
  static_assert(is_wMaxPacketSize<TwMaxPacketSize>(), "Not bInterfaceNumber record");
  static_assert(is_bInterval<TbInterval>(), "Not bInterfaceNumber record");
};

//==============================================================================
// Interface Association Descriptor Type
//==============================================================================
template<typename TbFirstInterface,
         typename TbInterfaceCount,
         typename TbFunctionClass,
         typename TbFunctionSubClass,
         typename TbFunctionProtocol,
         typename TiFunction>
struct INTERFACE_ASSOCIATION_DESCRIPTOR : public DESCRIPTOR<DescriptorType::INTERFACE_ASSOCIATION,
  TbFirstInterface, TbInterfaceCount, TbFunctionClass,
  TbFunctionSubClass, TbFunctionProtocol, TiFunction>
{
  static_assert(is_bFirstInterface<TbFirstInterface>(), "Not bFirstInterface record");
  static_assert(is_bInterfaceCount<TbInterfaceCount>(), "Not bInterfaceCount record");
  static_assert(is_bFunctionClass<TbFunctionClass>(), "Not bFunctionClass record");
  static_assert(is_bFunctionSubClass<TbFunctionSubClass>(), "Not bFunctionSubClass record");
  static_assert(is_bFunctionProtocol<TbFunctionProtocol>(), "Not bFunctionProtocol record");
  static_assert(is_iFunction<TiFunction>(), "Not iFunction record");
};

//==============================================================================
// CDC Header Functional Descriptor Type
//==============================================================================
template<typename TbDescriptorSubType,
         typename TbcdCDC>
struct CDC_HEADER_FUNCTIONAL_DESCRIPTOR : public DESCRIPTOR<DescriptorType::CS_INTERFACE,
  TbDescriptorSubType, TbcdCDC>, CDC_HEADER_FUNCTIONAL_DESCRIPTOR_BASE, DESCRIPTOR_BASE
{
  static_assert(is_bDescriptorSubType<TbDescriptorSubType>(), "Not bDescriptorSubType record");
  static_assert(is_bcdCDC<TbcdCDC>(), "Not bcdCDC record");
};

//==============================================================================
// CDC ACM Functional Descriptor Type
//==============================================================================
template<typename TbDescriptorSubType,
         typename TbmCapabilities>
struct CDC_ACM_FUNCTIONAL_DESCRIPTOR : public DESCRIPTOR<DescriptorType::CS_INTERFACE,
  TbDescriptorSubType, TbmCapabilities>, CDC_ACM_FUNCTIONAL_DESCRIPTOR_BASE, DESCRIPTOR_BASE
{
  static_assert(is_bDescriptorSubType<TbDescriptorSubType>(), "Not bFirstInterface record");
  static_assert(is_bmCapabilities<TbmCapabilities>(), "Not bmCapabilities record");
};

//==============================================================================
// CDC Union Functional Descriptor Type
//==============================================================================
template<typename TbDescriptorSubType,
         typename TbControlInterface,
         typename TbSubordinateInterface0>
struct CDC_UNION_FUNCTIONAL_DESCRIPTOR : public DESCRIPTOR<DescriptorType::CS_INTERFACE,
  TbDescriptorSubType, TbControlInterface, TbSubordinateInterface0>, 
  CDC_UNION_FUNCTIONAL_DESCRIPTOR_BASE, DESCRIPTOR_BASE
{
  static_assert(is_bDescriptorSubType<TbDescriptorSubType>(), "Not bFirstInterface record");
  static_assert(is_bControlInterface<TbControlInterface>(), "Not bControlInterface record");
  static_assert(is_bSubordinateInterface0<TbSubordinateInterface0>(), "Not bSubordinateInterface0 record");
};

//==============================================================================
// CDC Call Management Functional Descriptor Type
//==============================================================================
template<typename TbDescriptorSubType,
         typename TbmCapabilities,
         typename TbDataInterface>
struct CDC_CALL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR : public DESCRIPTOR<DescriptorType::CS_INTERFACE,
  TbDescriptorSubType, TbmCapabilities,
  TbDataInterface>, CDC_CALL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR_BASE, DESCRIPTOR_BASE
{
  static_assert(is_bDescriptorSubType<TbDescriptorSubType>(), "Not bFirstInterface record");
  static_assert(is_bmCapabilities<TbmCapabilities>(), "Not bmCapabilities record");
  static_assert(is_bDataInterface<TbDataInterface>(), "Not bDataInterface record");
};

//==============================================================================
// Custom HID Descriptor Type
//==============================================================================
template<typename TbcdHID,                // HID Version ( 1.11 )
         typename TbCountryCode,          // Код страны локализованного устройства
         typename TbNumDescriptors,       // Количество дескрипторов в классе
         typename TbDescriptorType_0,     // Тип дескриптора Report Descriptor
         typename TwDescriptorLength_0>   // Длина Report Descriptor
struct CUSTOM_HID_DESCRIPTOR : public DESCRIPTOR<DescriptorType::HID,
  TbcdHID, TbCountryCode, TbNumDescriptors,
  TbDescriptorType_0, TwDescriptorLength_0>, CUSTOM_HID_DESCRIPTOR_BASE
{
  static_assert(is_bcdHID<TbcdHID>(),                          "Not bcdHID record");
  static_assert(is_bCountryCode<TbCountryCode>(),              "Not bCountryCode record");
  static_assert(is_bNumDescriptors<TbNumDescriptors>(),        "Not bDataInterface record");
  static_assert(is_bDescriptorType_0<TbDescriptorType_0>(),    "Not bDescriptorType_0 record");
  static_assert(is_wDescriptorLength_0<TwDescriptorLength_0>(),"Not wDescriptorLength_0 record");
};
template<typename T> constexpr bool is_CustomHID() { return std::is_base_of_v<CUSTOM_HID_DESCRIPTOR_BASE,T>; }
