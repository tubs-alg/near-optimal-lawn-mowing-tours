#ifndef TSPN_MAP_HPP
#define TSPN_MAP_HPP

#include <vector>
#include <iterator>
#include <algorithm>
#include <functional>

namespace utils {

    template<class _Map>
    std::vector<typename _Map::mapped_type> valueset(const _Map &map) {
        std::vector<typename _Map::mapped_type> result;
        result.reserve(map.size());
        std::transform(map.cbegin(), map.cend(), std::back_inserter(result), [](typename _Map::const_reference kvpair) {
            return kvpair.second;
        });
        return result;
    }

    template<class _Map>
    std::vector<std::reference_wrapper<typename _Map::mapped_type>> valueset(_Map &map) {
        std::vector<std::reference_wrapper<typename _Map::mapped_type>> result;
        result.reserve(map.size());
        std::transform(map.begin(), map.end(), std::back_inserter(result), [](typename _Map::reference kvpair) {
            return std::ref(kvpair.second);
        });
        return result;
    }

    template<class _Map>
    std::vector<typename _Map::key_type> keyset(const _Map &map) {
        std::vector<typename _Map::key_type> result;
        result.reserve(map.size());
        std::transform(map.cbegin(), map.cend(), std::back_inserter(result), [](typename _Map::const_reference kvpair) {
            return kvpair.first;
        });
        return result;
    }
}
#endif //TSPN_MAP_HPP
