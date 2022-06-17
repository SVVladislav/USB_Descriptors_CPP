#pragma once

template<typename T>
struct TypeBox
{
  using type = T;
};

template<typename T>
using type_unbox = typename T::type;

template<auto Value>
struct ValueBox
{
  static constexpr auto value = Value;
};

template<typename T>
constexpr auto value_unbox() { return T::type::value; }

template<typename T>
constexpr auto value_unbox(T t) { return T::type::value; }

template<typename... Ts>
class TypeList 
{
public:
  static constexpr auto size() { return sizeof...(Ts); }
  
  static constexpr bool is_empty() { return (sizeof...(Ts) == 0); }
  
  static constexpr auto head() { return head_<Ts...>(); }
  
  static constexpr auto tail() { return tail_<Ts...>(); }
  
  static constexpr auto back() { return (TypeBox<Ts>{}, ...); }
  
  template<typename T>
  static constexpr auto push_front() { return TypeList<T, Ts...>{}; }

  template<typename T>
  static constexpr auto push_front(TypeBox<T>) { return push_front<T>(); }
  
  template<typename T>
  static constexpr auto push_back()  { return TypeList<Ts..., T>{}; }
  
  template<typename T>
  static constexpr auto push_back(TypeBox<T>) { return push_back<T>(); }
  
  template<typename T>
  static constexpr bool contains() { return (... || std::is_same_v<T, Ts>); }

  template<typename T>
  static constexpr bool contains(TypeBox<T>) { return contains<T>(); }
   
  template<typename F>
  static constexpr bool is_unique(F func) { return is_unique_<F, Ts...>(func); }

  static constexpr bool is_unique()
  {
    return is_unique([](auto v1, auto v2) { return std::is_same_v<type_unbox<decltype(v1)>, type_unbox<decltype(v2)>>; });
  }
  
  template<typename F>
  static constexpr auto accumulate(F func) { return (TypeList<>{} + ... + func(TypeBox<Ts>{})); }
  
  template<typename F>
  static inline void foreach(F func) { (func(TypeBox<Ts> {}), ...); }
  
  template<auto I, typename T>
  static constexpr auto generate() { return generate_<T>(std::make_index_sequence<I>{}); }
  
  template<typename F>
  static constexpr auto transform(F func) { return TypeList<type_unbox<decltype(func(TypeBox<Ts>{}))>...>{}; }
  
  //template<typename T>
  //static consteval auto filter(auto pred)
  //{
  //return (TypeList<>{} + ... + std::conditional_t<pred(TypeBox<T>{}, TypeBox<Ts>{}), TypeList<Ts>, TypeList<>>{});
  //}
  
  //template<typename T>
  //static consteval auto filter(TypeBox<T> box, auto pred) { return filter<T>(pred); }
  
  template<typename T>
  static constexpr auto filter(T pred)
  {
    return (TypeList<>{} + ... + std::conditional_t<pred(TypeBox<Ts>{}), TypeList<Ts>, TypeList<>>{});
  }
  
private:
  template<typename T, typename... Us>
  static constexpr auto head_() { return TypeBox<T>{}; }

  template<typename T, typename... Us>
  static constexpr auto tail_() { return TypeList<Us...>{}; }
  
  template<typename T, auto... Is>
  static constexpr auto generate_(std::index_sequence<Is...>) { return TypeList<type_unbox<decltype((Is, TypeBox<T>{}))>...>{}; }
  
  
  template<typename F>
  static constexpr bool is_unique_(F func) { return true; }

  template <typename F, typename T, typename... Us>
  static constexpr bool is_unique_(F &func)
  {
    return !(func(TypeBox<T>{}, TypeBox<Us>{}) || ...) && is_unique_<F, Us...>(func);
  }
 
};

template<typename... Ts, typename... Us>
constexpr bool operator == (TypeList<Ts...>, TypeList<Us...>) { return false; }

template<typename... Ts>
constexpr bool operator == (TypeList<Ts...>, TypeList<Ts...>) { return true; }

template<typename... Ts, typename... Us>
constexpr bool operator != (TypeList<Ts...>, TypeList<Us...>) { return true; }

template<typename... Ts>
constexpr bool operator != (TypeList<Ts...>, TypeList<Ts...>) { return false; }

template<typename... Ts, typename... Us>
constexpr auto operator + (TypeList<Ts...>, TypeList<Us...>) { return TypeList<Ts..., Us...> {}; }

