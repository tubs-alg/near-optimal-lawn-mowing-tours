//
// Created by Dominik Krupke on 15.01.23.
//
#include "cetsp/utils/geometry.h"
#include <cmath>
#include <utility>
namespace cetsp::utils {
double distance_to_segment(std::pair<double, double> A,
                           std::pair<double, double> B,
                           std::pair<double, double> E) {
  // stolen from
  // https://www.geeksforgeeks.org/minimum-distance-from-a-point-to-the-line-segment-using-vectors/
  using namespace std;
  // vector AB
  pair<double, double> AB;
  AB.first = B.first - A.first;
  AB.second = B.second - A.second;

  // vector BP
  pair<double, double> BE;
  BE.first = E.first - B.first;
  BE.second = E.second - B.second;

  // vector AP
  pair<double, double> AE;
  AE.first = E.first - A.first, AE.second = E.second - A.second;

  // Variables to store dot product
  double AB_BE, AB_AE;

  // Calculating the dot product
  AB_BE = (AB.first * BE.first + AB.second * BE.second);
  AB_AE = (AB.first * AE.first + AB.second * AE.second);

  // Minimum distance from
  // point E to the line segment
  double reqAns = 0;

  // Case 1
  if (AB_BE > 0) {

    // Finding the magnitude
    double y = E.second - B.second;
    double x = E.first - B.first;
    reqAns = sqrt(x * x + y * y);
  }

  // Case 2
  else if (AB_AE < 0) {
    double y = E.second - A.second;
    double x = E.first - A.first;
    reqAns = sqrt(x * x + y * y);
  }

  // Case 3
  else {

    // Finding the perpendicular distance
    double x1 = AB.first;
    double y1 = AB.second;
    double x2 = AE.first;
    double y2 = AE.second;
    double mod = sqrt(x1 * x1 + y1 * y1);
    reqAns = abs(x1 * y2 - y1 * x2) / mod;
  }
  return reqAns;
}
} // namespace cetsp::utils
