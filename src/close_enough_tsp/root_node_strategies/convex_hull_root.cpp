//
// Created by Dominik Krupke on 17.12.22.
//

#include "cetsp/details/cgal_kernel.h"
#include "cetsp/strategies/root_node_strategy.h"

#include <CGAL/Convex_hull_traits_adapter_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/property_map.h>
namespace cetsp {
std::shared_ptr<Node> ConvexHullRoot::get_root_node(Instance &instance) {
  if (instance.is_path()) {
    throw std::invalid_argument("ConvexHull Strategy only feasible for tours.");
  }
  typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
  typedef K::Point_2 Point_2;
  typedef CGAL::Convex_hull_traits_adapter_2<
      K, CGAL::Pointer_property_map<Point_2>::type>
      Convex_hull_traits_2;
  std::vector<Point_2> points;
  points.reserve(instance.size());
  for (const auto &c : instance) {
    points.emplace_back(c.center.x, c.center.y);
  }
  std::vector<int> indices(points.size()), out;
  std::iota(indices.begin(), indices.end(), 0);
  CGAL::convex_hull_2(indices.begin(), indices.end(), std::back_inserter(out),
                      Convex_hull_traits_2(CGAL::make_property_map(points)));
  std::vector<Circle> ch_circles;
  for (auto i : out) {
    ch_circles.push_back(instance[i]);
  }
  //  Only use circles that are  explicitly contained.
  const auto traj =
      compute_trajectory_with_information(ch_circles, /*path=*/false);
  std::vector<int> sequence;
  std::copy_if(out.begin(), out.end(), std::back_inserter(sequence),
               [&traj](auto i) { return traj.second[i]; });
  return std::make_shared<Node>(out, &instance);
}
} // namespace cetsp
