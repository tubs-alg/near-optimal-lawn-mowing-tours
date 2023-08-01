//
// Created by Barak Ugav on 20.01.23.
//

#ifndef CETSP_LAYERED_CONVEX_HULL_RULE_H
#define CETSP_LAYERED_CONVEX_HULL_RULE_H

#include "../../common.h"
#include "../../details/solution_pool.h"
#include "../../node.h"
#include "../rule.h"

namespace cetsp {

class ConvexHullLayer {
public:
  /*
   * Each convex hull vertex is assigned an number in range [0, CH size)
   * by their counter-clockwise order starting from an arbitrary one.
   * The first top most level CH is the 'regular' CH. The next layered CH is
   * achieved by removing all points that lie on the top most level CH, and
   * calculating the CH of the remaining sub set. This procedure is repeated as
   * long as there are some points not included in any layer. This class
   * represent a single such layer.
   */

  /* For each index p \in [0, n) of an input point, global_to_hull_map[p] is
   * present iff p is in the layer CH, and if it present the value is the CH
   * index of p. */
  std::vector<std::optional<unsigned int>> global_to_hull_map;
  /* For each CH index q, hull_to_global_map[q] is the global index of
   * q in the input. The size of hull_to_global_map is the number of vertices in
   * the layer CH. */
  std::vector<unsigned int> hull_to_global_map;

  bool is_in_hull(unsigned int i) const {
    auto opt = global_to_hull_map[i];
    return opt ? true : false;
  }

  static std::vector<ConvexHullLayer> calc_ch_layers(const Instance &instance);
};

class LayeredConvexHullRule : public SequenceRule {
public:
  void setup(const Instance *instance, std::shared_ptr<Node> &root,
             SolutionPool *solution_pool) override;

  bool is_ok(const std::vector<int> &seq, const Node &parent) override;
  bool is_ok(const std::vector<int> &seq) const;

  const ConvexHullLayer &get_layer(unsigned int layer_idx) const {
    assert(layer_idx < layers.size());
    return layers[layer_idx];
  }

  unsigned int get_number_of_layers() const { return layers.size(); }

private:
  bool is_ok(const std::vector<int> &seq, unsigned int layer) const;

  const Instance *instance = nullptr;
  std::vector<ConvexHullLayer> layers;
};

} // namespace cetsp
#endif // CETSP_LAYERED_CONVEX_HULL_RULE_H
