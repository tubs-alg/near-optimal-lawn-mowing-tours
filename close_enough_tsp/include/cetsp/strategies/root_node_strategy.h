//
// Created by Dominik Krupke on 14.12.22.
// The task of this file is to find the root node to start the BnB-algorithm
// with. For a tour, this can be three circles, as the order for three does
// not matter. For a path it is reasonably to start with the circle that is
// most distanced to both end points (we generally want the root node to be
// as expensive as possible and thus close to the cost of the feasible
// solutions)
//
// For Tours: Use Convex Hull Strategy
// For Paths: Use Longest Edge Plus Farthest Circle
//

#ifndef CETSP_ROOT_NODE_STRATEGY_H
#define CETSP_ROOT_NODE_STRATEGY_H
#include "cetsp/common.h"
#include "cetsp/node.h"
#include <random>
#include <vector>
namespace cetsp {

class RootNodeStrategy {
public:
  [[nodiscard]] virtual std::shared_ptr<Node>
  get_root_node(Instance &instance) = 0;
  virtual ~RootNodeStrategy() = default;
};

/**
 * The longest edge plus furthest circle-strategy, first selects the longest
 * edge and then the circle most distanced to it. Thus, for tours the
 * root solution will have three circles. Not that the order does not matter
 * for three circles. When using the convex hull strategy, the root node
 * may be illegal because of a bad rotation.
 *
 * For paths, this  strategy only returns the circle that is most distanced
 * to source and target (just sum of both distances).
 */
class LongestEdgePlusFurthestCircle : public RootNodeStrategy {
public:
  std::shared_ptr<Node> get_root_node(Instance &instance) override;
};

/**
 * The convex hull-strategy selects all circles on the convex hull as
 * root solution that are not implicitly covered. So first, the solution
 * on all circles intersecting the convex  hull is computed and then all
 * circles that span the solution selected.
 * This approach  IS NOT compatible with paths.
 * This approach IS compatible with the convex hull order pruning.
 */
class ConvexHullRoot : public RootNodeStrategy {
public:
  std::shared_ptr<Node> get_root_node(Instance &instance) override;
};

class RandomRoot : public RootNodeStrategy {
public:
  std::shared_ptr<Node> get_root_node(Instance &instance) override {
    if (instance.is_path()) {
      if (instance.empty()) {
        return std::make_shared<Node>(std::vector<int>{}, &instance);
      }
      std::vector<int> seq;
      std::default_random_engine generator;
      std::uniform_int_distribution<int> distribution(0, instance.size() - 1);
      seq.push_back(static_cast<int>(distribution(generator)));
      return std::make_shared<Node>(seq, &instance);
    } else {
      if (instance.size() <= 3) { // trivial case
        std::vector<int> seq;
        for (int i = 0; i < static_cast<int>(instance.size()); ++i) {
          seq.push_back(i);
        }
        return std::make_shared<Node>(seq, &instance);
      }
      std::vector<int> seq(instance.size());
      std::iota(seq.begin(), seq.end(), 0);
      std::shuffle(seq.begin(), seq.end(),
                   std::mt19937{std::random_device{}()});
      seq.resize(3);
      return std::make_shared<Node>(seq, &instance);
    }
  }
};
} // namespace cetsp
#endif // CETSP_ROOT_NODE_STRATEGY_H
