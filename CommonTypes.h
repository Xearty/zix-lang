#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

using String = std::string;

template <typename T>
using Vector = std::vector<T>;

template <typename Key, typename T>
using HashMap = std::unordered_map<Key, T>;

template <typename T>
using SharedPtr = std::shared_ptr<T>;

template <typename T, class... Args>
inline SharedPtr<T> MakeShared(Args&&... args) {
    return std::make_shared<T, Args...>(std::forward<Args>(args)...);
};

