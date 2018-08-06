#ifndef SRC_CHROMAVEC_MVDF_H_
#define SRC_CHROMAVEC_MVDF_H_

#include <cstdint>
#include <vector>

#include <opencv2/core.hpp>

#include "utilities/filter.h"
#include "utilities/rgbvector.h"

namespace chromavec { namespace internal {

/**
 * @brief Implementation of a Minimum Vector Dispersion Filter.
 */
class VMFilter : public OperatorBase<CV_8UC3, CV_8UC3>
{
public:
    /**
     * @brief Construct a new filter object.
     * @param width
     *      filter window width
     * @raises std::runtime_error
     *      if the window width is not an odd value
     */
    VMFilter(const int width);

    /**
     * @brief Override of the '()' operator.
     * @param x, y
     *      the current pixel
     * @param img
     *      image being processed
     * @return
     *      output colour
     */
    RGBVector<uint8_t> operator()(const int x, const int y, const cv::Mat &img);

    // Default copy-and-assign
    VMFilter(const VMFilter &) = default;
    VMFilter &operator=(const VMFilter &) = default;

private:
    const int width_;
};

}} // namespace chromavec::internal

#endif // SRC_CHROMAVEC_MVDF_H_
