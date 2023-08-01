#include "approximation/FeketeApproximation.h"

namespace approximation {
    FeketeApproximation::FeketeApproximation(Polygon_2 &polygon, double radius, double time) {
        this->straight_line_polygon = polygon;

        if (this->straight_line_polygon.is_clockwise_oriented()) {
            this->straight_line_polygon.reverse_orientation();
        }

        this->radius = radius;
        this->time = time;
    }

    FeketeApproximation::solution FeketeApproximation::solve() {
        using std::chrono::high_resolution_clock;
        using std::chrono::duration;
        using std::chrono::milliseconds;

        auto result = solution{this->straight_line_polygon,
                               PointVector(),
                               PointVector(),
                               this->radius,
                               0,
                               0, false, 0};



        auto points = this->generateHexagonalGrid();
        std::cout << "Generated " << points.size() << " points" << std::endl;

        auto t1 = high_resolution_clock::now();
        auto tsp_solver = tsp::TSPSolver(points, this->time);
        auto solution = tsp_solver.solve();
        auto t2 = high_resolution_clock::now();

        result.points = points;
        result.runtime = ((duration<double, std::milli>) (t2 - t1)).count();
        result.solved_optimally = solution.solved_optimally;
        result.tour = solution.points;
        result.upper_bound = solution.upper_bound;
        result.lower_bound = solution.lower_bound;

        return result;
    }

    FeketeApproximation::PointVector FeketeApproximation::generateHexagonalGrid() {
        PointVector hexagonalGrid;
        auto bbox = this->straight_line_polygon.bbox();
        auto xMin = bbox.xmin() - 5 * this->radius, yMin = bbox.ymin() - 5 * this->radius,
                yMax = bbox.ymax() + 5 * this->radius, xMax = bbox.xmax() + 5 * this->radius;

        auto side_length = this->radius * std::sqrt(3);
        auto r = 0;
        auto y = yMin;
        auto y_step_length = this->radius * 1.5;

        // cos(pi/6) = cos(60deg) = sqrt(3)/2

        while (y <= yMax) {
            auto start_x = r % 2 == 0 ? xMin : xMin + 0.5 * side_length;


            for (auto x = start_x; x <= xMax; x += side_length) {
                auto p = Point(x, y);

                Polygon_2 hexagon;
                hexagon.push_back(Point(x, y+this->radius));
                hexagon.push_back(Point(x - side_length/2, y+this->radius/2));
                hexagon.push_back(Point(x - side_length/2, y-this->radius/2));
                hexagon.push_back(Point(x, y-this->radius));
                hexagon.push_back(Point(x + side_length/2, y-this->radius/2));
                hexagon.push_back(Point(x + side_length/2, y+this->radius/2));


                if(CGAL::do_intersect(hexagon, this->straight_line_polygon)) {
                    hexagonalGrid.emplace_back(p);
                }

                //x += side_length;
            }

            y += y_step_length;
            r += 1;
        }

        return hexagonalGrid;
    }
}