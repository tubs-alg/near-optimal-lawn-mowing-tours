#ifndef LAWN_MOWING_WITNESS_SPARSIFICATION_H
#define LAWN_MOWING_WITNESS_SPARSIFICATION_H

#include "mowing/utils/definitions.h"

namespace mowing {

    class WitnessSparsification {
    public:
        typedef mowing::definitions::PointVector PointVector;
        typedef mowing::definitions::ConicPolygon ConicPolygon;
        typedef mowing::definitions::InputPolygon InputPolygon;

        class WitnessSparsificationException : public std::exception {
        private:
            std::string message_;
        public:
            explicit WitnessSparsificationException(std::string message) : message_(std::move(message)) {};

            [[nodiscard]] const char *what() const noexcept override {
                return message_.c_str();
            }
        };

        // Sparsify can return a point vector for logging.
        virtual std::optional<PointVector> sparsify(PointVector &witnesses,
                                                    std::size_t n,
                                                    const std::optional<ConicPolygon> &region = std::nullopt,
                                                    const std::optional<InputPolygon> &inputPolygon = std::nullopt) = 0;

        virtual ~WitnessSparsification() = default;
    };
}
#endif
