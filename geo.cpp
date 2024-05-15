#define _USE_MATH_DEFINES
#include "geo.h"

#include <cmath>

    namespace geo
    {
        bool IsZero(double value) {
            return std::abs(value) < EPSILON;
        }

        double ComputeDistance(Coordinates from, Coordinates to) 
        {
            using namespace std;
            const double dr = M_PI / 180.0;
            return acos(sin(from.lat * dr) * sin(to.lat * dr) + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr)) * 6371000;
        }

        svg::Point SphereProjector::operator()(geo::Coordinates coords) const
        {
            return { (coords.lng - min_lon_) * zoom_coeff_ + padding_, (max_lat_ - coords.lat) * zoom_coeff_ + padding_ };
        }

    }  // namespace geo