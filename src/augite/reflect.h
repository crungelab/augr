#pragma once
// C++17+
//
// Minimal base-class registry + RTTR-like macros (no Self argument).
// Inside a class, write:
//   struct Widget   { REFLECT_ENABLE(); };
//   struct Button : Widget { REFLECT_ENABLE(Widget); }
//
// Register once per concrete type (in a .cpp) with:
//   REFLECT_REGISTER(Button);


#include <typeindex>
#include <type_traits> // for std::decay_t
#include <unordered_map>
#include <vector>
#include <mutex>
#include <type_traits>

namespace reflect {

// ------------------------- Tiny base graph ------------------------------
class Registry {
public:
  static Registry& instance() {
    static Registry r;
    return r;
  }

  template<class T, class... Bases>
  void register_type() {
    std::lock_guard<std::mutex> lock(m_);
    auto& bases = graph_[std::type_index(typeid(T))];
    (append_unique(bases, std::type_index(typeid(Bases))), ...);
  }

  // Immediate bases of a type (empty if unknown).
  const std::vector<std::type_index>& bases_of(const std::type_index& t) const {
    static const std::vector<std::type_index> kEmpty;
    if (auto it = graph_.find(t); it != graph_.end()) return it->second;
    return kEmpty;
  }

private:
  static void append_unique(std::vector<std::type_index>& v, const std::type_index& ti) {
    for (const auto& x : v) if (x == ti) return;
    v.push_back(ti);
  }

  std::unordered_map<std::type_index, std::vector<std::type_index>> graph_;
  mutable std::mutex m_;
};

// ---------------------- Compile-time base list --------------------------
// Classes with REFLECT_ENABLE(...) declare a nested alias __reflect_bases.
// We use it to register types without repeating base names elsewhere.

template<class... Ts> struct type_list {};

template<class T, class = void>
struct bases_of { using type = type_list<>; };

template<class T>
struct bases_of<T, std::void_t<typename T::__reflect_bases>> {
  using type = typename T::__reflect_bases;
};

// Forward decl
template<class T> inline void ensure_registered();

// Helper to unpack a type_list and recurse into bases first.
template<class T, class List> struct register_recursive;

template<class T, class... Bases>
struct register_recursive<T, type_list<Bases...>> {
  static void go() {
    // 1) Recursively ensure bases are registered,
    //    so their own edges (e.g., Base->GrandBase) exist.
    (ensure_registered<Bases>(), ...);

    // 2) Now register T with its immediate bases (edge list).
    Registry::instance().template register_type<T, Bases...>();
  }
};

template<class T>
inline void ensure_registered() {
  // One-time guard per T, prevents cycles/duplicates
  static const bool once = (register_recursive<T, typename bases_of<T>::type>::go(), true);
  (void)once;
}

// Touch and get_type (as you already added)
template<class T>
inline void touch(const T& obj) {
  obj.__reflect_touch(); // injected by REFLECT_ENABLE(...)
}

template<class T>
inline std::type_index get_type(const T& obj) {
  touch(obj);                           // ensures most-derived is registered
  return std::type_index(typeid(obj));  // returns dynamic type
}

} // namespace reflect

// --------------------------- Macros -------------------------------------
//
// Put inside class definitions. No Self parameter.
//   REFLECT_ENABLE()              // no bases
//   REFLECT_ENABLE(Base1, Base2)  // immediate bases only
//

/*
#define REFLECT_ENABLE(...) \
public: \
  using __reflect_bases = ::reflect::type_list<__VA_ARGS__>;
*/

// Place inside class: REFLECT_ENABLE(Base1, Base2, ...)
// or REFLECT_ENABLE() for no bases.
#define REFLECT_ENABLE(...) \
public: \
  using __reflect_bases = ::reflect::type_list<__VA_ARGS__>; \
  /* Force registration of the *concrete* type at runtime via virtual dispatch. */ \
  virtual void __reflect_touch() const { \
    using Self = std::decay_t<decltype(*this)>; \
    ::reflect::ensure_registered<Self>(); \
  }

// Register outside the class (e.g., in a .cpp).
#define REFLECT_REGISTER(T) \
  namespace { \
  [[maybe_unused]] inline const bool __reflect_reg_##T = [](){ \
    ::reflect::ensure_registered<T>(); \
    return true; \
  }(); \
  }
