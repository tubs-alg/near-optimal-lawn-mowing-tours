#include "mowing/witnesses/placement/followup/StraightSkeletonPlacementStrategy.h"

namespace mowing {
    void StraightSkeletonPlacementStrategy::placeWitnesses(PointVector &witnesses,
                                                           [[maybe_unused]]const std::optional<std::size_t> &n,
                                                           [[maybe_unused]]const std::optional<ConicPolygon> &region) {

        if (!region) {
            throw WitnessPlacementStrategy::WitnessPlacementException("Straight skeleton strategy requires region");
        }

        std::vector<Point> points = std::vector<Point>();

        CGAL::Polygon_2<Epick> poly;
        auto exactPolygon = mowing::utils::approximate_polygon(*region);
        for (auto it = exactPolygon.vertices_begin(); it != exactPolygon.vertices_end(); it++) {
            poly.push_back(Epick::Point_2(CGAL::to_double(it->x()), CGAL::to_double(it->y())));
            points.emplace_back(*it);
        }


        if (poly.is_simple()) {
            this->addInteriorWitnesses(points, poly, true);

            auto skeleton = CGAL::create_interior_straight_skeleton_2(poly.vertices_begin(), poly.vertices_end());

            for (auto it = skeleton->halfedges_begin(); it != skeleton->halfedges_end(); it++) {
                if (it->is_bisector()) {
                    points.emplace_back(it->opposite()->vertex()->point().x(),
                                        it->opposite()->vertex()->point().y());
                    points.emplace_back(it->vertex()->point().x(), it->vertex()->point().y());
                }
            }

            mowing::utils::get_points_along_half_edges(skeleton->halfedges_begin(),
                                                       skeleton->halfedges_end(),
                                                       this->radius, points);
        } else {
            mowing::utils::get_points_along_edges(poly.edges_begin(), poly.edges_end(), this->radius, points);
        }


        this->cleanUpAndAddWitnesses(points, witnesses);

    }

    void
    StraightSkeletonPlacementStrategy::addInteriorWitnesses(std::vector<Point> &witnesses, CGAL::Polygon_2<Epick> &P,
                                                            bool add_edges) {
        if (P.is_clockwise_oriented()) {
            P.reverse_orientation();
        }

        if (add_edges) mowing::utils::get_points_along_edges(P.edges_begin(), P.edges_end(), this->radius, witnesses);

        for (auto it = P.vertices_begin(); it != P.vertices_end(); it++) {
            witnesses.emplace_back(it->x(), it->y());
        }

        /*
        auto offset_polygons = CGAL::create_interior_skeleton_and_offset_polygons_2(this->radius, P);

        for (auto &offset_polygon: offset_polygons) {
            this->addInteriorWitnesses(witnesses, *offset_polygon, false);
        }*/
    }
}

