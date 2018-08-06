/**
 * @file
 * @brief Application-wide constants.
 * @author Richard Rzeszutek
 * @date July 2, 2018
 */
#ifndef SRC_CHROMAVEC_CONSTANTS_H_
#define SRC_CHROMAVEC_CONSTANTS_H_

#include <cmath>

namespace chromavec { namespace internal {

constexpr double kSqrt2 = 1.4142135623730951;
constexpr double kPi = 3.141592653589793;
constexpr int kMaxDistanceSq = 3*255*255;
constexpr int kMaxDistance = 441; // floor(sqrt(3*255*255))

}} // namespace chromavec::internal

#endif // SRC_CHROMAVEC_CONSTANTS_H_
