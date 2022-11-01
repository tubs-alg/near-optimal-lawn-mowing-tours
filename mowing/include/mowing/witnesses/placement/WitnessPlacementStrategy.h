#ifndef LAWN_MOWING_WITNESS_PLACEMENT_STRATEGY_H
#define LAWN_MOWING_WITNESS_PLACEMENT_STRATEGY_H

#include <utility>
#include <vector>
#include "utils/utils.hpp"
#include "mowing/utils/definitions.h"
#include "mowing/utils/conversion.h"
#include "mowing/utils/mowing.h"

namespace mowing {
    /**
    * This class is a base class for witness placement strategies.
    */
    class WitnessPlacementStrategy {
    public:
        typedef mowing::definitions::ConicPolygon ConicPolygon;
        typedef mowing::definitions::InputPolygon InputPolygon;
        typedef mowing::definitions::PointVector PointVector;

        class WitnessPlacementException: public std::exception {
        private:
            std::string message_;
        public:
            explicit WitnessPlacementException(std::string message) : message_(std::move(message)) {};
            [[nodiscard]] const char* what() const noexcept override {
                return message_.c_str();
            }
        };

        WitnessPlacementStrategy(InputPolygon &inputPolygon, double radius);
        WitnessPlacementStrategy();

        virtual void placeWitnesses(PointVector &witnesses,
                                    const std::optional<std::size_t> &n,
                                    const std::optional<ConicPolygon> &region) = 0;

        virtual ~WitnessPlacementStrategy() = default;
    protected:
        InputPolygon inputPolygon;
        double radius;

        void cleanUpAndAddWitnesses(PointVector &points, PointVector &witnesses);
    };

}

#endif
