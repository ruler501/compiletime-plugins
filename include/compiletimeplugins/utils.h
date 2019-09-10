#ifndef _MTSPARKED_UTILS_H_
#define _MTSPARKED_UTILS_H_

#include <new>
#include <type_traits>

#include "unconstexpr/tools/type_list.h"

template <typename... Args>
using type_list = unconstexpr::details::type_list<Args...>;

template <typename T>
struct type_value {};

template <size_t size, size_t align>
class initializing_buffer {
private:
    union type
    {
        std::byte __data[size];
#ifdef _MSC_VER
        struct __declspec(align(align)) {} __align;
#else // _MSC_VER
        struct __attribute__((__aligned__((align)))) { } __align;
#endif // _MSC_VER
    } value;

    using deletePtrType = void(*)(void*);
    deletePtrType deletePtr;

public:
    template <typename T, typename... Args>
    initializing_buffer(type_value<T>, Args&&... args) : deletePtr([](void* memory){ delete static_cast<T*>(memory);}) {
        new (value.__data) T{std::forward<Args>(args)...};
        static_assert(sizeof(T) <= size);
        static_assert(alignof(T) <= align && align % alignof(T) == 0);
    }

    initializing_buffer(const initializing_buffer& other) = delete;
    initializing_buffer(initializing_buffer&& other) = delete;

    constexpr operator void*() { // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
        return std::launder(value.__data);
    }

    constexpr operator const void*() const { // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
        return std::launder(value.__data);
    }
};

template<typename Result, typename... Args>
struct callablePlugin {
    template<typename Buffer>
    struct type {
        using func=Result(*)(Args...);
        func fPtr;
        template<typename T>
            callablePlugin(type_value<T>) 
            : fPtr([](void* obj, Args... args){ return static_cast<T*>(obj)->operator()(args...); })
        { }
        Result (Args... args) override {
            return fPtr(static_cast<void*>(static_cast<Buffer&>(*this)), args...);
        }
    };
};

template<template<typename> typename... Features>
struct erased : public Features<erased<Features...>>..., public initializing_buffer<8, 8> {
    template<typename T, typename... Args>
    erased(type_value<T> t, Args&&... args) : initializing_buffer(t, std::forward<Args>(args)...), Features<erased<Features...>>(t)... 
        { }
};
#endif // !_MTSPARKED_UTILS_H_
