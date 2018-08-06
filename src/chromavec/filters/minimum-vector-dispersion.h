/**
 * @file
 * @author Richard Rzeszutek
 * @date July 1, 2018
 */
#ifndef SRC_CHROMAVEC_FILTERS_MINIMUM_VECTOR_DISPERSION_H_
#define SRC_CHROMAVEC_FILTERS_MINIMUM_VECTOR_DISPERSION_H_

#include <cstdint>
#include <vector>

#include <opencv2/core.hpp>

#include <tbb/cache_aligned_allocator.h>

#include "utilities/filter.h"
#include "utilities/rgbvector.h"

namespace chromavec { namespace internal {

/**
 * @brief Implementation of a Minimum Vector Dispersion Filter.
 */
class MinVecDispersionFilter : public OperatorBase<CV_8UC3, CV_8UC3>
{
public:
    /**
     * @brief Construct a new filter object.
     * @param width
     *      filter window width
     * @param k, l
     *      filtering parameters used to control the trade-off between noise
     *      suppression or edge detection
     * @raises std::runtime_error
     *      if the window width is not an odd value or if the filter parameters
     *      produce an invalid filter
     */
    MinVecDispersionFilter(const int width, const int k, const int l);

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
    MinVecDispersionFilter(const MinVecDispersionFilter &) = default;
    MinVecDispersionFilter &operator=(const MinVecDispersionFilter &) = default;

private:
    const int k_, l_;
    const int width_;
    std::vector<int, tbb::cache_aligned_allocator<int>> distances_;
    std::vector<int, tbb::cache_aligned_allocator<int>> indices_;
};

}} // namespace chromavec::internal

#endif // SRC_CHROMAVEC_FILTERS_MINIMUM_VECTOR_DISPERSION_H_
