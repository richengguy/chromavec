#include "vmf.h"

#include <algorithm>
#include <stdexcept>

#include "constants.h"

#include "utilities/functions.h"
#include "utilities/roi.h"
#include "utilities/rgbvector.h"

namespace chromavec { namespace internal {

VMFilter::VMFilter(const int width)
    : width_(width)
{
    if (width < 3 || (width % 2) == 0)
        throw std::runtime_error("Filter width must be odd.");
}

RGBVector<uint8_t> VMFilter::operator()(const int x, const int y, const cv::Mat &img)
{
    const internal::ROI window(img, x, y, this->width_);
    const int width = window.Width();
    const int height = window.Height();

    RGBVector<uint8_t> best_vector(window(0, 0), img.channels());
    int minimum_distance = kMaxDistance*this->width_*this->width_;

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

            // Check to see if it's the best distance and update if it is.
            if (distance < minimum_distance)
            {
                minimum_distance = distance;
                best_vector = pi;
            }
        }

    // Output is the magnitude between the least central and most central
    // vectors.
    return best_vector;
}

}} // namespace chromavec::internal
