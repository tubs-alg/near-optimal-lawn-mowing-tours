//
// Created by Dominik Krupke on 11.12.22.
// This file provides some simple CGAL definitions to ease the use of CGAL.
//

#ifndef SAMPLNS_CGAL_KERNEL_H
#define SAMPLNS_CGAL_KERNEL_H
#include <CGAL/Cartesian.h>
#include <CGAL/Point_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Segment_2.h>
#include <CGAL/squared_distance_2.h>
namespace cetsp {
namespace details {
using cgKernel = CGAL::Cartesian<double>;
using cgPoint = CGAL::Point_2<cgKernel>;
using cgSegment = CGAL::Segment_2<cgKernel>;
using cgPolygon = CGAL::Polygon_2<cgKernel>;
} // namespace details
} // namespace cetsp
#endif // SAMPLNS_CGAL_KERNEL_H
