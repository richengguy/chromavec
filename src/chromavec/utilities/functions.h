#ifndef SRC_CHROMAVEC_UTILITIES_H_
#define SRC_CHROMAVEC_UTILITIES_H_

#include <algorithm>
#include <iterator>
#include <numeric>
#include <type_traits>
#include <utility>
#include <vector>

#include <opencv2/core.hpp>

#include "constants.h"
#include "rgbvector.h"

namespace chromavec { namespace internal {

/**
 * @brief Perform an "argsort" operation on a list of indices.
 * @param [in] vfirst, vlast
 *      random-access iterators to the first and last elements in the list used
 *      to decide how the indices are sorted
 * @param [out] ifirst, ilast
 *      random-access iterators to the first and last elements in the list used
 *      to store the sorted indices
 * @raises std::runtime_error
 *      if the index list is not long enough to store the set of values or if
 *      the number of valid
 */
template<typename Value, typename Index>
void ArgSort(Value vfirst, Value vlast, Index ifirst, Index ilast)
{
    typedef typename std::iterator_traits<Value>::value_type value_type;
    typedef typename std::iterator_traits<Index>::value_type index_type;

    static_assert(std::is_integral<index_type>::value,
                  "Index type must be integral type (e.g. int).");

    auto val_len = std::distance(vfirst, vlast);
    auto ind_len = std::distance(ifirst, ilast);

    if (val_len != ind_len)
        throw std::runtime_error("Not enough space allocated for the sorted indices.");

    std::iota(ifirst, ilast, 0);
    std::sort(ifirst, ilast, [&vfirst](const value_type &a, const value_type &b) -> bool
              {
                  return *(vfirst + a) < *(vfirst + b);
              });
}

/**
 * @brief Clamp an (x,y) coordinate to be within an image.
 * @param img
 *      image (used to obtain the dimensions)
 * @param x, y
 *      image pixel coordinate
 */
inline std::pair<int, int> ClampCoordinate(const cv::Mat &img,
                                           const int x, const int y)
{
    return std::pair<int, int>(
        std::clamp(x, 0, img.cols-1),
        std::clamp(y, 0, img.rows-1)
    );
}

/**
 * The gradient is defined to be the magnitude of the vector difference between
 * two pixels in some window.  Because the magnitude is always positive, the
 * true edge direction is unknown.  Therefore, there is no difference between
 * the convolution kernel `[-1 0 1]` and `[1 0 -1]`. However, the *tangent* to
 * that vector will always point along the edge itself and is a valid isophote.
 *
 * @brief Compute a colour gradient.
 * @tparam dx, dy
 *      the 'x' and 'y' offset from the central pixel
 * @param img
 *      RGB colour image
 * @param x, y
 *      current filtering location
 * @return
 *      squared distance between colour vectors
 */
template<int dx, int dy>
int ComputeGradient(const cv::Mat &img, const int x, const int y)
{
    const auto c1 = ClampCoordinate(img, x - dx, y - dy);
    const auto c2 = ClampCoordinate(img, x + dx, y + dy);

    const RGBVector<uint8_t> p1(img, c1.first, c1.second);
    const RGBVector<uint8_t> p2(img, c2.first, c2.second);

    return p1.SquaredDistance(p2);
}

/**
 * @brief Convert radians into degrees.
 * @param rad
 *      radians
 * @return
 *      degrees
 */
constexpr double RadiansToDegrees(const double rad)
{
    return rad * 180.0 / kPi;
}

/**
 * @brief Convert degrees into radians.
 * @param deg
 *      degrees
 * @return
 *      radians
 */
constexpr double DegreesToRadians(const double deg)
{
    return deg * kPi / 180.0;
}

}} // namespace chromavec::internal

#endif // SRC_CHROMAVEC_UTILITIES_H_
