#pragma once

#include <stdexcept>
#include <algorithm>
#include <map>


template<typename Key, typename Value, std::size_t Size>
struct Map
{
    std::array<std::pair<Key, Value>, Size> data;

    constexpr Value at(const Key& key) const
    {
        const auto itr = std::find_if(begin(data), end(data)), [&key](const auto& v) { return v.first == key; });
        if (itr != end)
            return itr->second;
        else
            throw std::range_error("Not found");

    }
};
