#include "vector-range.h"

#include <cmath>
#include <stdexcept>

#include "constants.h"

#include "utilities/filter.h"
#include "utilities/functions.h"
#include "utilities/roi.h"

namespace chromavec { namespace internal {

VectorRangeFilter::VectorRangeFilter(const int width)
    : width_(width)
{
    if (width < 3 || (width % 2) == 0)
        throw std::runtime_error("Filter width must be odd.");
}

RGBVector<uint8_t> VectorRangeFilter::operator()(const int x, const int y,
                                                 const cv::Mat &img)
{
    const internal::ROI window(img, x, y, this->width_);
    const int width = window.Width();
    const int height = window.Height();

    // Keep track of the maximum and minimum distances.
    int min_distance = kMaxDistanceSq*this->width_*this->width_;
    int max_distance = 0;

    RGBVector<uint8_t> min_colour(window[0], img.channels());
    RGBVector<uint8_t> max_colour(window[0], img.channels());

    // This is two separate loops: one over all of the pixels in the window and
    // another to compute the aggregate distances between the current window
    // pixel and all other pixels.
    for (int yi = 0; yi < height; yi++)
        for (int xi = 0; xi < width; xi++)
        {
            // Compute the aggregate distances for the current pixel.
            const RGBVector<uint8_t> pi(window(xi, yi), img.channels());
            int distance = 0;
            for (int yj = 0; yj < height; yj++)
                for (int xj = 0; xj < width; xj++)
                {
                    const RGBVector<uint8_t> pj(window(xj, yj), img.channels());
                    distance += pi.SquaredDistance(pj);
                }

            // Update the minimum/maximum distance values.
            if (distance < min_distance)
            {
                min_distance = distance;
                min_colour = pi;
            }

            if (distance > max_distance)
            {
                max_distance = distance;
                max_colour = pi;
            }
        }

    // Output is the scaled magnitude between the two extracted vectors.
    const int sqdist = min_colour.SquaredDistance(max_colour);
    const uint8_t value = 255*std::sqrt(static_cast<double>(sqdist))/kMaxDistance;

    return RGBVector<uint8_t>(value, value, value);
}

}} // namespace chromavec::internal
