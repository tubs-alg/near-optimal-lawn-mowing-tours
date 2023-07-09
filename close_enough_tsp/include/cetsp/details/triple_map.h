/**
 * The tripple map is used to approximate the costs of a tour  by lazily
 * saving  the minimal costs  of connecting tripples. It seems to yield
 * reasonably good values.
 */

#ifndef CETSP_TRIPPLE_MAP_H
#define CETSP_TRIPPLE_MAP_H
#include "cetsp/common.h"
#include "cetsp/soc.h"
#include <functional>
#include <unordered_map>

namespace cetsp {
using Triple = std::tuple<int, int, int>;
}
template <> struct std::hash<cetsp::Triple> {
  std::size_t operator()(const cetsp::Triple &k) const {
    auto [u, v, w] = k;
    return ((hash<int>()(u) ^ (hash<int>()(v) << 1)) >> 1) ^
           (hash<int>()(w) << 1);
  }
};

namespace cetsp {

class TripleMap {
public:
  TripleMap(Instance *instance) : instance{instance} {}
  double get_cost(int u, int v, int w) {
    if (w < u) { // making triple unique independ of direction
      std::swap(u, w);
    }
    Triple uvw(u, v, w);
    if (map.count(uvw) > 0) {
      return map[uvw];
    }
    auto l = compute_cost(u, v, w);
    map[uvw] = l;
    return l;
  }

  double estimate_cost_for_sequence(const std::vector<int> &seq) {
    double c = 0.0;
    if (instance->is_tour()) {
      for (unsigned i = 0; i < seq.size(); ++i) {
        c += get_cost(i, (i + 1) % seq.size(), (i + 2) % seq.size());
      }
    } else {
      if (seq.empty()) {
        return instance->path->first.dist(instance->path->second);
      } else if (seq.size() == 1) {
        return get_cost(-1, seq[0], -2);
      } else {
        c += get_cost(-1, seq[0], seq[1]);
        for (unsigned i = 0; i < seq.size() - 2; ++i) {
          c += get_cost(i, (i + 1), (i + 2));
        }
        c += get_cost(seq[seq.size() - 2], seq[seq.size() - 1], -2);
      }
    }
    return c;
  }

private:
  double compute_cost(int u, int v, int w) {
    std::vector<Circle> seq{get_circle(u), get_circle(v), get_circle(w)};
    auto l = compute_tour(seq, true).length();
    return l / 2;
  }

  Circle get_circle(int i) {
    if (i == -1) {
      assert(instance->is_path());
      return Circle(instance->path->first, 0);
    } else if (i == -2) {
      assert(instance->is_path());
      return Circle(instance->path->second, 0);
    }
    return (*instance)[i];
  }

  Instance *instance;
  std::unordered_map<Triple, double> map;
};

} // namespace cetsp
#endif // CETSP_TRIPPLE_MAP_H
