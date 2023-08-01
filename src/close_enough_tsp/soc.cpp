#include "cetsp/soc.h"
#include "cetsp/common.h"
#include <gurobi_c++.h>
#include <vector>
namespace cetsp {

std::pair<Trajectory, std::vector<bool>>
compute_trajectory_with_information(const std::vector<Circle> &circle_sequence,
                                    bool path) {
  constexpr auto SPANNING_TOLERANCE = 0.01;

  static GRBEnv env;
  GRBModel model(&env);

  const auto n = circle_sequence.size();
  std::vector<GRBVar> x;
  x.resize(n);
  std::vector<GRBVar> y;
  y.resize(n);
  std::vector<GRBVar> f;
  f.resize(n);
  std::vector<GRBVar> w;
  w.resize(n);
  std::vector<GRBVar> u;
  u.resize(n);
  std::vector<GRBVar> s;
  s.resize(n);
  std::vector<GRBVar> t;
  t.resize(n);
  GRBLinExpr obj = 0;

  for (unsigned i = 0; i < n; ++i) {
    x[i] = model.addVar(/*lb=*/-GRB_INFINITY, /*ub=*/GRB_INFINITY,
                        /*obj=*/0.0, /*type=*/GRB_CONTINUOUS);
    y[i] = model.addVar(/*lb=*/-GRB_INFINITY, /*ub=*/GRB_INFINITY,
                        /*obj=*/0.0, /*type=*/GRB_CONTINUOUS
                        /*name=*/);
    f[i] = model.addVar(/*lb=*/0, /*ub=*/GRB_INFINITY,
                        /*obj=*/0.0, /*type=*/GRB_CONTINUOUS
                        /*name=*/);
    w[i] = model.addVar(/*lb=*/-GRB_INFINITY, /*ub=*/GRB_INFINITY,
                        /*obj=*/0.0, /*type=*/GRB_CONTINUOUS
                        /*name=*/);
    u[i] = model.addVar(/*lb=*/-GRB_INFINITY, /*ub=*/GRB_INFINITY,
                        /*obj=*/0.0, /*type=*/GRB_CONTINUOUS
                        /*name=*/);
    s[i] = model.addVar(/*lb=*/-GRB_INFINITY, /*ub=*/GRB_INFINITY,
                        /*obj=*/0.0, /*type=*/GRB_CONTINUOUS
                        /*name=*/);
    t[i] = model.addVar(/*lb=*/-GRB_INFINITY, /*ub=*/GRB_INFINITY,
                        /*obj=*/0.0, /*type=*/GRB_CONTINUOUS
                        /*name=*/);
    obj += f[i];
  }

  model.setObjective(obj, GRB_MINIMIZE);

  for (unsigned i = 0; i < n; ++i) {
    model.addQConstr(f[i] * f[i] >= w[i] * w[i] + u[i] * u[i]);
    const auto r = circle_sequence[i].radius;
    model.addQConstr(s[i] * s[i] + t[i] * t[i] <= r * r);

    const auto cx = circle_sequence[i].center.x;
    const auto cy = circle_sequence[i].center.y;
    model.addConstr(s[i] == cx - x[i]);
    model.addConstr(t[i] == cy - y[i]);
  }

  for (unsigned i = 0; i < n; ++i) {
    if (path && i == 0) {
      model.addConstr(w[i] == 0);
      model.addConstr(u[i] == 0);
    } else {
      const auto prev_c = (i == 0 ? n - 1 : i - 1);
      assert(prev_c >= 0);
      model.addConstr(w[i] == x[prev_c] - x[i]);
      model.addConstr(u[i] == y[prev_c] - y[i]);
    }
  }
  model.set(GRB_IntParam_OutputFlag, 0);
  // tuned via the built-in tune() function of Gurobi.
  model.set(GRB_IntParam_Presolve, 0);
  model.set(GRB_IntParam_SimplexPricing, 3);
  // model.set(GRB_IntParam_PrePasses, 8);
  model.optimize();
  std::vector<Point> points;
  points.reserve(n + 1);
  std::vector<bool> spanning_circles(n);
  for (unsigned i = 0; i < n; i++) {
    points.emplace_back(x[i].get(GRB_DoubleAttr_X), y[i].get(GRB_DoubleAttr_X));
    const auto si = s[i].get(GRB_DoubleAttr_X);
    const auto ti = t[i].get(GRB_DoubleAttr_X);
    const auto r = circle_sequence[i].radius;
    bool is_spanning =
        std::sqrt(si * si + ti * ti) >= (1 - SPANNING_TOLERANCE) * r;
    spanning_circles[i] = is_spanning;
  }
  if (!path) {
    points.push_back(points[0]);
  }
  return {Trajectory(points), spanning_circles};
}

Trajectory compute_tour(const std::vector<Circle> &circle_sequence,
                        const bool path) {
  return compute_trajectory_with_information(circle_sequence, path).first;
}
} // namespace cetsp
