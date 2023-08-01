/**
 * Computing the distance to a trajectory seems to be an expensive operation,
 * so we cache it.
 */
#ifndef CETSP_DISTANCE_CACHE_H
#define CETSP_DISTANCE_CACHE_H
#include "../common.h"
#include <vector>
namespace cetsp::details {

class DistanceCache {
  /**
   * Just a simple cached distance calculator.
   * TODO: Improve runtime. Current implementation of just using the trajectory
   * method is slow.
   */
public:
  explicit DistanceCache(const Instance *instance) : instance{instance} {}

  double operator()(int i, const Trajectory *trajectory) {
    assert(i < static_cast<int>(instance->size()));
    if (i >= static_cast<int>(cache.size())) {
      fill_cache(trajectory);
    }
    return cache[i];
  }

  const Instance *instance;

private:
  void fill_cache(const Trajectory *trajectory) {
    cache.reserve(instance->size());
    for (unsigned i = cache.size(); i < instance->size(); ++i) {
      cache.push_back(trajectory->distance((*instance)[i]));
    }
  }

  std::vector<double> cache;
};

} // namespace cetsp::details
#endif // CETSP_DISTANCE_CACHE_H
