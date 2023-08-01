#ifndef CETSP_SOLVER_H
#define CETSP_SOLVER_H

#include "cetsp/bnb.h"
#include "cetsp/common.h"
#include "cetsp/details/cross_lower_bound.h"
#include "cetsp/details/triple_map.h"
#include "cetsp/heuristics.h"
#include "cetsp/node.h"
#include "cetsp/strategies/rules/global_convex_hull_rule.h"
#include "cetsp/strategies/rules/layered_convex_hull_rule.h"

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>

typedef CGAL::Exact_predicates_exact_constructions_kernel CGALKernel;
typedef CGALKernel::Point_2 CGALPoint;

struct cetsp_solution {
    double lower_bound;
    double upper_bound;
    std::vector<CGALPoint> points;
    bool optimal_solution_found;
};

namespace cetsp {
    inline cetsp_solution solve(std::vector<CGALPoint> &points,
                         const std::shared_ptr<CGALPoint> &start_point,
                         double radius,
                         double time) {
        auto instance = Instance();

        // If the start point is given we pass it as an initial point. Else use the default solver without a start.
        if (start_point) {
            auto center = Point(CGAL::to_double(start_point->x()), CGAL::to_double(start_point->y()));
            auto circle = Circle(center, 0);
            instance.add_circle(circle);
        }

        for (auto &p: points) {
            auto center = Point(CGAL::to_double(p.x()), CGAL::to_double(p.y()));
            auto circle = Circle(center, radius);
            instance.add_circle(circle);
        }

        auto rns = std::make_unique<ConvexHullRoot>();

        auto branching_strategy = std::make_unique<ChFarthestCircle>(false,
                                                                     std::thread::hardware_concurrency());

        auto search_strategy = std::make_unique<CheapestChildDepthFirst>();

        branching_strategy->add_rule(std::make_unique<GlobalConvexHullRule>());
        branching_strategy->add_rule(std::make_unique<LayeredConvexHullRule>());

        BranchAndBoundAlgorithm baba(&instance, rns->get_root_node(instance),
                                     *branching_strategy, *search_strategy);

        baba.add_upper_bound(compute_tour_by_2opt(instance));

        auto gap = 0.01;
        baba.optimize((int) time, gap);
        auto result_points = std::vector<CGALPoint>();

        auto trajectory = baba.get_solution()->get_trajectory();
        for (const auto &p : trajectory.points) {
            result_points.emplace_back(p.x, p.y);
        }

        return cetsp_solution{baba.get_lower_bound(),
                              baba.get_upper_bound(),
                              result_points,
                              baba.get_upper_bound() <= (1 + gap) * baba.get_lower_bound()
        };
    }
}
#endif
