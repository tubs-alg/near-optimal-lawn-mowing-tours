//
// Created by Dominik Krupke on 21.12.22.
//
#include "cetsp/strategies/branching_strategy.h"
#include <boost/thread/thread.hpp>
// #include <execution>
namespace cetsp {

/**
 * Find the circle that is most distanced to the current (relaxed) solution.
 * This is a good circle to branch upon. If no circle is uncovered, it returns
 * nothing.
 * @param solution The relaxed solution.
 * @param instance The instance with the circles.
 * @return The index of the most distanced circle in the solution or nothing
 *          if all circles are included.
 */
std::optional<int>
get_index_of_most_distanced_circle(const PartialSequenceSolution &solution,
                                   const Instance &instance) {
  const int n = static_cast<int>(instance.size());
  std::vector<double> distances(n);
  for (int i = 0; i < n; ++i) {
    if (solution.covers(i)) {
      distances[i] = 0;
    } else {
      distances[i] = solution.distance(i);
    }
  }
  auto max_dist = std::max_element(distances.begin(), distances.end());
  if (*max_dist <= 0) {
    return {};
  }
  const int c = static_cast<int>(std::distance(distances.begin(), max_dist));
  return {c};
}

void distributed_child_evaluation(std::vector<std::shared_ptr<Node>> &children,
                                  const bool simplify,
                                  const size_t num_threads) {
  // Parallelize the computation of the relaxed solutions for the children
  // using a simple modulo on the number of threads. This is fine as we write
  // on separate heap memory for all children.
  boost::thread_group tg;
  if (num_threads <= 1) { // Without threading overhead.
    for (auto i = 0; i < children.size(); i += 1) {
      children[i]->trigger_lazy_evaluation();
      if (simplify) {
        children[i]->simplify();
      }
    }
  } else {
    for (unsigned int offset = 0;
         offset < std::min(num_threads, children.size()); ++offset) {
      tg.create_thread([=, &children]() {
        for (auto i = offset; i < children.size(); i += num_threads) {
          children[i]->trigger_lazy_evaluation();
          if (simplify) {
            children[i]->simplify();
          }
        }
      });
    }
  }
  tg.join_all(); // Wait for all threads to finish, so we are in a consistent
                 // state.
}

bool CircleBranching::branch(Node &node) {
  const auto c = get_branching_circle(node);
  if (!c) {
    return false;
  }
  std::vector<std::shared_ptr<Node>> children;
  std::vector<int> seq;
  seq = node.get_fixed_sequence();
  seq.push_back(*c);
  if (instance->is_path()) {
    // for path, this position may not be symmetric and has to be added.
    if (is_sequence_ok(seq, node)) {
      children.push_back(std::make_shared<Node>(seq, instance, &node));
    }
  }
  for (int i = seq.size() - 1; i > 0; --i) {
    seq[i] = seq[i - 1];
    seq[i - 1] = *c;
    if (is_sequence_ok(seq, node)) {
      children.push_back(std::make_shared<Node>(seq, instance, &node));
    }
  }
  distributed_child_evaluation(children, simplify, num_threads);
  node.branch(children);
  return true;
}

std::optional<int> FarthestCircle::get_branching_circle(Node &node) {
  const auto c = get_index_of_most_distanced_circle(node.get_relaxed_solution(),
                                                    *instance);
  return c;
}
std::optional<int> RandomCircle::get_branching_circle(Node &node) {
  std::vector<int> uncovered_circles;
  for (unsigned i = 0; i < instance->size(); ++i) {
    if (!node.get_relaxed_solution().covers(i)) {
      uncovered_circles.push_back(i);
    }
  }
  if (uncovered_circles.empty()) {
    return {};
  }
  std::default_random_engine generator;
  std::uniform_int_distribution<int> distribution(0,
                                                  uncovered_circles.size() - 1);
  return {uncovered_circles[distribution(generator)]};
}
} // namespace cetsp
