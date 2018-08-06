/**
 * @file
 * @author Richard Rzeszutek
 * @date June 29, 2018
 */
#ifndef SRC_CHROMAVEC_UTILITIES_FILTER_H_
#define SRC_CHROMAVEC_UTILITIES_FILTER_H_

#include <cstdint>
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include <opencv2/core.hpp>

#include <tbb/blocked_range2d.h>
#include <tbb/blocked_range3d.h>
#include <tbb/parallel_for.h>

#include "rgbvector.h"

namespace chromavec { namespace internal {

/**
 * @brief Base template used to define C++ to OpenCV type mapping.
 * @note See template specializations for supported types.
 */
template<int OcvType>
struct OpenCVTypeInfo { };

/**
 * @brief Generate the template specializations for the support OpenCV types.
 */
#define CHROMAVEC_DEFINE_TYPE(OcvType, Type, Channels) \
template<>\
struct OpenCVTypeInfo<OcvType> \
{ \
    typedef Type type; \
    static constexpr int channels = Channels; \
};

CHROMAVEC_DEFINE_TYPE(CV_8UC1, uint8_t, 1)  ///< 8-bit, single channel
CHROMAVEC_DEFINE_TYPE(CV_8UC3, uint8_t, 3)  ///< 8-bit, three channel
CHROMAVEC_DEFINE_TYPE(CV_32SC1, int32_t, 1)     ///< 32-bit signed integer, single channel
CHROMAVEC_DEFINE_TYPE(CV_32SC3, int32_t, 3)     ///< 32-bit signed integer, three channels
CHROMAVEC_DEFINE_TYPE(CV_32FC1, float, 1)   ///< 32-bit floating point, single channel
CHROMAVEC_DEFINE_TYPE(CV_32FC3, float, 3)   ///< 32-bit floating point, three channels

#undef CHROMAVEC_DEFINE_TYPE

/**
 * @brief Helper used to define a basic filter operation.
 * @tparam InputType
 *      the input type
 * @tparam OutputType
 *      the generated output type
 */
template<int InputType, int OutputType>
struct OperatorBase
{
    static constexpr int input_type = InputType;
    static constexpr int output_type = OutputType;
};

template<typename Operator, typename ...Args>
void Filter(cv::Mat &filtered, const cv::Mat &img, Args &&...args)
{
    typedef typename OpenCVTypeInfo<Operator::output_type>::type out_type;

    if (img.type() != Operator::input_type)
        throw std::runtime_error("Input type not supported by this filter.");

    if (filtered.type() != Operator::output_type)
        throw std::runtime_error("Output type not supported by this filter.");

    const int channels = OpenCVTypeInfo<Operator::output_type>::channels;

    // Apply the filter across all pixels in the image.  The model assumes that
    // each (x,y) position will produce a single RGB value that is then stored
    // in the output image.
    const tbb::blocked_range2d<int> range(0, img.rows, 0, img.cols);
    tbb::parallel_for<tbb::blocked_range2d<int>>(
        range,
        [&](const tbb::blocked_range2d<int> &block)
        {
            // Construct the filtering operator (in case it allocates some of
            // its own memory for internal buffers).
            Operator op(std::forward<Args>(args)...);

            const int x_start = block.cols().begin();
            const int x_end = block.cols().end();

            const int y_start = block.rows().begin();
            const int y_end = block.rows().end();

            for (int y = y_start; y != y_end; y++)
            {
                auto row = filtered.ptr<out_type>(y);
                for (int x = x_start; x != x_end; x++)
                {
                    const int i = channels*x;

                    // Perform the filtering operation.
                    RGBVector output = op(x, y, img);

                    // Insert pixel values based on the number of channels by
                    // exploiting how a switch-case statement works.
                    switch(channels)
                    {
                        case 4:
                        case 3:
                            row[i+2] = output.blue;
                        case 2:
                            row[i+1] = output.green;
                        case 1:
                            row[i] = output.red;
                    }
                }
            }
        }
    );
}

/**
 * @brief Apply a filter onto an image.
 * @tparam Operator
 *      the operator object that will perform the filtering
 * @param img
 *      the image being filtered
 * @param args
 *      any arguments that will be passed into the filtering operator
 * @return
 *      the filtered image
 * @throws std::runtime_error
 *      if the input image doesn't have 8-bit depth
 */
template<typename Operator, typename ...Args>
cv::Mat Filter(const cv::Mat &img, Args &&...args)
{
    if (img.type() != Operator::input_type)
        throw std::runtime_error("Input type not supported by this filter.");

    // Create an output image.
    cv::Mat outimg = cv::Mat::zeros(img.rows, img.cols, Operator::output_type);
    Filter<Operator>(outimg, img, std::forward<Args>(args)...);
    return outimg;
}

}} // namespace chromavec::internal

#endif // SRC_CHROMAVEC_UTILITIES_WINDOWPROC_H_
