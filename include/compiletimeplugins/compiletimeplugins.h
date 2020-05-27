template <typename T>
struct injectable_t {
    static constexpr bool value = false;
};

template<typename T>
concept injectable = injectable_t<T>::value;

template<typename T> requires injectable<T>
struct injector;
