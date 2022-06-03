#pragma once

template<typename T>
using TypeBox = std::type_identity<T>;

template<auto value>
using TypeUnBox = typename decltype(value)::type;

template <auto Value>
struct ValueBox
{
  static constexpr auto value = Value;
};

template<typename T>
constexpr auto value_unbox() { return T::type::value; }

constexpr auto value_unbox(auto t) { return decltype(t)::type::value; }

template<typename... Ts>
class TypeList
{
public:
  static consteval auto size() { return sizeof...(Ts); }

  static consteval bool is_empty() { return (sizeof...(Ts) == 0); }

  static consteval auto head()
  {
    return []<typename T, typename... Us>(TypeList<T, Us...>){ return TypeBox<T>{}; }(TypeList<Ts...>{});
  }

  static consteval auto tail()
  {
    return[]<typename T, typename... Us>(TypeList<T, Us...>) { return TypeList<Us...>{}; }(TypeList<Ts...>{});
  }
 
  static consteval auto back() { return (TypeBox<Ts>{}, ...); }

  template<typename T>
  static consteval auto push_front() { return TypeList<T, Ts...>{}; }

  template<typename T>
  static consteval auto push_front(TypeBox<T>) { return push_front<T>(); }

  template<typename T>
  static consteval auto push_back()  { return TypeList<Ts..., T>{}; }

  template<typename T>
  static consteval auto push_back(TypeBox<T>) { return push_back<T>(); }

  template<typename T>
  static consteval bool contains() { return (... || std::is_same_v<T, Ts>); }

  template<typename T>
  static consteval bool contains(TypeBox<T>) { return contains<T>(); }

  static consteval bool is_unique(auto func) { return is_unique_<Ts...>(func); }

  static consteval bool is_unique()
  {
    return isUnique([](auto v1, auto v2) { return std::is_same_v<TypeUnBox<v1>, TypeUnBox<v2>>; });
  }

  static consteval auto accumulate(auto func) { return (TypeList<>{} + ... + func(TypeBox<Ts>{})); }
 
  static inline void foreach(auto func) { (func(TypeBox<Ts> {}), ...); }

  template<typename T>
  static consteval auto filter(auto pred)
  {
    return (TypeList<>{}+ ... +std::conditional_t<pred(TypeBox<T>{}, TypeBox<Ts>{}),TypeList<Ts>,TypeList<>>{});
  }

  template<typename T>
  static consteval auto filter(TypeBox<T> box, auto pred) { return filter<T>(pred); }
	
  static consteval auto filter(auto pred)
  {
    return (TypeList<>{} + ... + std::conditional_t<pred(TypeBox<Ts>{}), TypeList<Ts>, TypeList<>>{});
  }
  
  template<auto I, typename T>
  static consteval auto generate()
  {
    return []<auto... Is>(std::index_sequence<Is...>)
           {
             return TypeList<TypeUnBox<(Is, TypeBox<T>{})>...>{};
           } (std::make_index_sequence<I>());
  }
  
private:
  static consteval bool is_unique_(auto func) { return true; }

  template <typename T, typename... Us>
  static consteval bool is_unique_(auto func)
  {
    return !(func(TypeBox<T>{}, TypeBox<Us>{}) || ...) && is_unique_<Us...>(func);
  }
};

template<typename... Ts, typename... Us>
consteval bool operator ==(TypeList<Ts...>, TypeList<Us...>) { return false; }

template<typename... Ts>
consteval bool operator ==(TypeList<Ts...>, TypeList<Ts...>) { return true; }

template<typename... Ts, typename... Us>
consteval bool operator !=(TypeList<Ts...>, TypeList<Us...>) { return true; }

template<typename... Ts>
consteval bool operator !=(TypeList<Ts...>, TypeList<Ts...>) { return false; }

template<typename... Ts, typename... Us>
consteval auto operator+(TypeList<Ts...>, TypeList<Us...>) { return TypeList<Ts..., Us...> {}; }
