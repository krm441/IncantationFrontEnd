#pragma once
#include <memory>

template<typename T>
using Ptr = std::shared_ptr<T>;

template<typename T, class... Args>
Ptr<T> create(Args... args)
{
    return std::make_shared<T>(args...);
}

template <typename T>
using UPtr = std::unique_ptr<T>;

template<typename T, class... Args>
UPtr<T> createUnique(Args... args)
{
    return std::make_unique<T>(args...);
}