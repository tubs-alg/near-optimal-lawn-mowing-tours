#include "mowing/utils/cetsp_tours.h"

namespace mowing::utils {
        template<typename Polygon>
        bool equal(Polygon &poly1, Polygon &poly2) {
            if (poly1.size() != poly2.size()) return false;

            auto current_vertex_poly1 = poly1.vertices_circulator();
            auto end_poly1 = current_vertex_poly1;

            bool startVertexPresent = false;
            auto current_vertex_poly2 = poly2.vertices_circulator();
            auto end_poly2 = current_vertex_poly2;
            do {
                if (*current_vertex_poly1 == *current_vertex_poly2) {
                    startVertexPresent = true;
                    break;
                }
            } while (++current_vertex_poly2 != end_poly2);

            if (!startVertexPresent) return false;

            end_poly2 = current_vertex_poly2;

            do {
                if (*current_vertex_poly1 != *current_vertex_poly2) return false;
            } while (++current_vertex_poly1 != end_poly1 && ++current_vertex_poly2 != end_poly2);

            return true;
        }

        template bool equal(CGAL::Polygon_2<CGAL::Cartesian<CGAL::CORE_algebraic_number_traits::Rational>> &poly1,
                            CGAL::Polygon_2<CGAL::Cartesian<CGAL::CORE_algebraic_number_traits::Rational>> &poly2);

        template<typename PointType>
        void cleanup_tour(std::vector<PointType> &tour) {
            // Save the last point of the tour for the case that all points are removed.
            auto tour_end = tour.back();

            for (auto it = tour.begin(); it != tour.end();) {
                auto next = it + 1;
                if (next == tour.end()) next = tour.begin();

                if (*it == *next) {
                    it = tour.erase(it);
                } else {
                    it++;
                }
            }

            for (auto it = tour.begin(); it != tour.end();) {
                if (tour.size() < 3) {
                    break;
                }

                auto next = it + 1;
                if (next == tour.end()) next = tour.begin();

                auto afterNext = next + 1;
                if (afterNext == tour.end()) afterNext = tour.begin();

                if (CGAL::collinear(*it, *next, *afterNext) && *it != *afterNext && *it != *next &&
                    *next != *afterNext) {
                    it = tour.erase(next);
                } else {
                    it++;
                }
            }

            for (auto it = tour.begin(); it != tour.end();) {
                auto next = it + 1;
                if (next == tour.end()) next = tour.begin();

                if (*it == *next) {
                    it = tour.erase(it);
                } else {
                    it++;
                }
            }


            if (tour.empty()) { // All tour points were the same, thus we add one point back.
                tour.emplace_back(tour_end);
            }
        }

        template void cleanup_tour(std::vector<Point> &tour);
        template void cleanup_tour(std::vector<CGAL::Cartesian<Nt_traits::Rational>::Point_2> &tour);
    }

