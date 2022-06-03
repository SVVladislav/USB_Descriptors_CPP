#pragma once

namespace HID_REPORT {
namespace PRIV {

enum class ITEM_TYPE : uint8_t { // Main Items
                                 Input=0x80, Output=0x90, Feature=0xB0, Collection=0xA0, EndCollection=0xC0,
                                 // Global Items
                                 UsagePage=0x04, LogicalMin=0x14, LogicalMax=0x24, PhysicalMin=0x34,
                                 PhysicalMax=0x44, UnitExponent=0x54, Unit=0x64,
                                 ReportSize=0x74, ReportID=0x84, ReportCount=0x94,
                                 // Local Items
                                 Usage=0x08, UsageMin=0x18, UsageMax=0x28,
                                 DesignatorIndex=0x38, DesignatorMin=0x48, DesignatorMax=0x58,
                                 StringIndex=0x68, StringMin=0x78, StringMax=0x88
                               };

template <uint8_t... data>
struct U8_DATA
{
  uint8_t buf[sizeof...(data)]{data...};
};

template<typename TRCRD>
constexpr auto copy_buf(TRCRD rcrd, uint8_t **p)
{
  for(auto &x : rcrd.buf) *(*p)++ = x;
}

template <typename... RCRDS>
struct BUF_COLLECTOR
{
  constexpr BUF_COLLECTOR()
  {
    uint8_t *p = buf;
    (copy_buf(RCRDS{}, &p),...);
  }
  uint8_t buf[(sizeof(RCRDS::buf)+...)]{};
};

template<ITEM_TYPE it, int32_t value>
class ITEM_SIGN_VALUE
{
  static constexpr uint32_t S2U()
  {
    if constexpr  (value>=0) return value;
    else if constexpr (value>=-128 )return ((0xFFFF'FFFFU-uint32_t(-value))+1) & 0xFF;
    else if constexpr (value>=-32768 )return ((0xFFFF'FFFFU-uint32_t(-value))+1) & 0xFFFF;
    else return (0xFFFF'FFFFU-uint32_t(-value))+1;
  }
  static constexpr auto CalcBytes()
  {
    if constexpr (value>=0)
      if constexpr (value>0x0000'7FFF) return 4;
      else if constexpr (value>0x0000'007F) return 2;
      else return 1;
    else
      if constexpr (S2U()>0x0000'FFFF) return 4;
      else if constexpr (S2U()>0x0000'00FF) return 2;
      else return 1;
  }
  static constexpr auto sz=CalcBytes()+1;
public:
  constexpr ITEM_SIGN_VALUE()
  {
    auto x = S2U();
    buf[0] = (uint8_t)it + ((sz<4)?(sz-1):3);
    for(auto i=0; i<sz-1; x>>=8) buf[1+i++]=x;
  }
  static constexpr auto Value() { return value; }
  uint8_t buf[sz]{};
};

template<ITEM_TYPE it, uint32_t value, uint32_t value_mask=0xFFFF'FFFF>
class ITEM_UNSIGN_VALUE
{
  static_assert( (value&~value_mask)==0, " Reserved bits check");
  static constexpr auto CalcBytes()
  {
    if constexpr (value>0x0000'FFFF) return 4;
    else if constexpr (value>0x0000'00FF) return 2;
    else return 1;
  }
  static constexpr auto sz=CalcBytes()+1;
public:
  constexpr ITEM_UNSIGN_VALUE()
  {
    auto x = value;
    buf[0] = (uint8_t)it + ((sz<4)?(sz-1):3);
    for(auto i=0; i<sz-1; x>>=8) buf[1+i++]=x;
  }
  static constexpr auto Value() { return value; }
  uint8_t buf[sz]{};
};

template<typename TMIN, typename TMAX>
class ITEM_MINMAX
{
  static_assert(TMIN::Value()<TMAX::Value(),"MIN < MAX");
  using T = PRIV::BUF_COLLECTOR<TMIN,TMAX>;
public:
  constexpr ITEM_MINMAX()
  {
    uint8_t *p = buf;
    PRIV::copy_buf(T{}, &p);
  }
  uint8_t buf[sizeof(T::buf)]{};
};

enum class COLLECTIONS : uint8_t { Physical=0, Application=1, Logical=2, Report=3,
                                   NamedArray=4, UsageSwitch=5, UsageModifier=6 };

template <COLLECTIONS cl, typename... RCRDS>
using COLLECTION = PRIV::BUF_COLLECTOR<ITEM_UNSIGN_VALUE<ITEM_TYPE::Collection,(uint8_t)cl>,
                                       RCRDS..., PRIV::U8_DATA<(uint8_t)ITEM_TYPE::EndCollection>>;
} // namespace PRIV

template <typename... RCRDS> using HID_REPORT_DESCRIPTOR = PRIV::BUF_COLLECTOR<RCRDS...>;

// Main Items
template<uint32_t data> using Input = PRIV::ITEM_UNSIGN_VALUE<PRIV::ITEM_TYPE::Input, data, 0x17F>;
template<uint32_t data> using Output = PRIV::ITEM_UNSIGN_VALUE<PRIV::ITEM_TYPE::Output, data, 0x1FF>;
template<uint32_t data> using Feature = PRIV::ITEM_UNSIGN_VALUE<PRIV::ITEM_TYPE::Feature, data, 0x1FF>;
//   Collections
template<typename... RCRDS> using COLLECTION_PHYSICAL = PRIV::COLLECTION<PRIV::COLLECTIONS::Physical, RCRDS...>;
template<typename... RCRDS> using COLLECTION_APPLICATION = PRIV::COLLECTION<PRIV::COLLECTIONS::Application, RCRDS...>;
template<typename... RCRDS> using COLLECTION_LOGICAL = PRIV::COLLECTION<PRIV::COLLECTIONS::Logical, RCRDS...>;
template<typename... RCRDS> using COLLECTION_REPORT = PRIV::COLLECTION<PRIV::COLLECTIONS::Report, RCRDS...>;
template<typename... RCRDS> using COLLECTION_NAMEDARRAY = PRIV::COLLECTION<PRIV::COLLECTIONS::NamedArray, RCRDS...>;
template<typename... RCRDS> using COLLECTION_USAGESWITCH = PRIV::COLLECTION<PRIV::COLLECTIONS::UsageSwitch, RCRDS...>;
template<typename... RCRDS> using COLLECTION_USAGEMODIFIER = PRIV::COLLECTION<PRIV::COLLECTIONS::UsageModifier, RCRDS...>;
// Global Items
enum class USAGE_PAGE : int16_t { UNDEFINED=0, GENERIC_DESCTOP=1, SIMULATION=2, VR=3, SPORT=4, GAME=5,
                                  DEV_CONTROLS=6, KEYBOARD=7, LED=8, BUTTON=9, ORDINALS=0xA,
                                  TELEPHONY_DEVICE=0xB, CONSUMER_DEVICE=0xC, DIGITIZER=0xD,
                                  UNICODE=0x10, ALPHANUMERIC_DISPLAY=0x14, MONITOR=-128,
                                  MONITOR_ENUMERATED_VALUES=-127, VESA_VIRTUAL_CONTROLS=-126,
                                  VESA_COMMAND=-125, POWER_DEVICE=-124, BATTARY_SYSTEM=-123,
                                  VENDOR_DEFINED_PAGE_1=-256 };
template<USAGE_PAGE data> using UsagePage = PRIV::ITEM_SIGN_VALUE<PRIV::ITEM_TYPE::UsagePage, (int16_t)data>;
template<int32_t data> using LogicalMin = PRIV::ITEM_SIGN_VALUE<PRIV::ITEM_TYPE::LogicalMin, data>;
template<int32_t data> using LogicalMax = PRIV::ITEM_SIGN_VALUE<PRIV::ITEM_TYPE::LogicalMax, data>;
template<int32_t min, int32_t max> using LogicalMinMax = PRIV::ITEM_MINMAX<LogicalMin<min>, LogicalMax<max>> ;
template<int32_t data> using PhysicalMin = PRIV::ITEM_SIGN_VALUE<PRIV::ITEM_TYPE::PhysicalMin, data>;
template<int32_t data> using PhysicalMax = PRIV::ITEM_SIGN_VALUE<PRIV::ITEM_TYPE::PhysicalMax, data>;
template<int32_t min, int32_t max> using PhysicalMinMax = PRIV::ITEM_MINMAX<PhysicalMin<min>,PhysicalMax<max>>;
template<uint32_t data> using ReportSize = PRIV::ITEM_UNSIGN_VALUE<PRIV::ITEM_TYPE::ReportSize, data>;
template<uint8_t data> using ReportID = PRIV::ITEM_UNSIGN_VALUE<PRIV::ITEM_TYPE::ReportID, data>;
template<uint32_t data> using ReportCount = PRIV::ITEM_UNSIGN_VALUE<PRIV::ITEM_TYPE::ReportCount, data>;
template<uint32_t sz, int32_t cnt> using ReportFormat = PRIV::BUF_COLLECTOR<ReportSize<sz>,ReportCount<cnt>>;   // ???
using PUSH = PRIV::U8_DATA<0xA4>;
using POP = PRIV::U8_DATA<0xB4>;

// Local Items
template<uint8_t data> using Usage = PRIV::ITEM_UNSIGN_VALUE<PRIV::ITEM_TYPE::Usage, data>;
template<uint8_t data> using UsageMin = PRIV::ITEM_UNSIGN_VALUE<PRIV::ITEM_TYPE::UsageMin, data>;
template<uint8_t data> using UsageMax = PRIV::ITEM_UNSIGN_VALUE<PRIV::ITEM_TYPE::UsageMax, data>;
template<uint8_t min, uint8_t max> using UsageMinMax = PRIV::ITEM_MINMAX<UsageMin<min>,UsageMax<max>>;
template<uint32_t data> using DesignatorIndex = PRIV::ITEM_UNSIGN_VALUE<PRIV::ITEM_TYPE::DesignatorIndex, data>;
template<uint32_t data> using DesignatorMin = PRIV::ITEM_UNSIGN_VALUE<PRIV::ITEM_TYPE::DesignatorMin, data>;
template<uint32_t data> using DesignatorMax = PRIV::ITEM_UNSIGN_VALUE<PRIV::ITEM_TYPE::DesignatorMax, data>;
template<uint32_t min, uint32_t max> using DesignatorMinMax = PRIV::ITEM_MINMAX<DesignatorMin<min>,DesignatorMax<max>>;
template<uint32_t data> using StringIndex = PRIV::ITEM_UNSIGN_VALUE<PRIV::ITEM_TYPE::StringIndex, data>;
template<uint32_t data> using StringMin = PRIV::ITEM_UNSIGN_VALUE<PRIV::ITEM_TYPE::StringMin, data>;
template<uint32_t data> using StringMax = PRIV::ITEM_UNSIGN_VALUE<PRIV::ITEM_TYPE::StringMax, data>;
template<uint32_t min, uint32_t max> using StringMinMax = PRIV::ITEM_MINMAX<StringMin<min>,StringMax<max>>;


} // namespace HID_REPORT