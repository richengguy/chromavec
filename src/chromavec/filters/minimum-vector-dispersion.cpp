#include "minimum-vector-dispersion.h"

#include <stdexcept>

#include "constants.h"
#include "utilities/functions.h"
#include "utilities/roi.h"

namespace chromavec { namespace internal {

MinVecDispersionFilter::MinVecDispersionFilter(const int width, const int k,
                                               const int l)
    : k_(k),
      l_(l),
      width_(width),
      distances_(width*width),
      indices_(width*width)
{
    const int N = width*width;
    if (width < 3 || (width % 2) == 0)
        throw std::runtime_error("Filter width must be odd.");
    if (k >= N || l >= N)
    {
        throw std::runtime_error("'k' and 'l' cannot be greater than the "
                                 "number of pixels in the filter window.");
    }
    if (k < 1 || l < 1)
    {
        throw std::runtime_error("'k' and 'l' must be non-zero.");
    }
}

RGBVector<uint8_t> MinVecDispersionFilter::operator()(const int x, const int y,
                                                      const cv::Mat &img)
{
    const internal::ROI window(img, x, y, this->width_);
    const int width = window.Width();
    const int height = window.Height();

    // Count the number of elements actually added into the window (handle the
    // edges gracefully).
    int N = 0;

    // This is two separate loops: one over all of the pixels in the window and
    // another to compute the aggregate distances between the current window
    // pixel and all other pixels.
    for (int yi = 0; yi < height; yi++)
        for (int xi = 0; xi < width; xi++)
        {
            // Compute the aggregate distances for the current pixel.
            const RGBVector<uint8_t> pi(window(xi, yi), img.channels());
            this->distances_[N] = 0;
            for (int yj = 0; yj < height; yj++)
                for (int xj = 0; xj < width; xj++)
                {
                    const RGBVector<uint8_t> pj(window(xj, yj), img.channels());
                    this->distances_[N] += pi.SquaredDistance(pj);
                }

            // Update the number of distances.
            N++;
        }

    // Figure out the order of the distances used an argsort.
    ArgSort(std::begin(this->distances_), std::begin(this->distances_) + N,
            std::begin(this->indices_), std::begin(this->indices_) + N);

    // Compute the MVDF output but comparing the set of least similar vectors to
    // the average of the most similar ones.  First, compute the average of the
    // 'l' most similar vectors.
    uint32_t sum_r = 0;
    uint32_t sum_g = 0;
    uint32_t sum_b = 0;

    for (int i = 0; i < this->l_; i++)
    {
        const uint8_t *pixel = window[this->indices_[i]];
        switch(img.channels())
        {
            case 4:
            case 3:
                sum_b += pixel[2];
            case 2:
                sum_g += pixel[1];
            case 1:
                sum_r += pixel[0];
        }
    }

    sum_r /= this->l_;
    sum_g /= this->l_;
    sum_b /= this->l_;

    const RGBVector<uint8_t> mean_rgb(std::clamp<uint32_t>(sum_r, 0, 255),
                                      std::clamp<uint32_t>(sum_g, 0, 255),
                                      std::clamp<uint32_t>(sum_b, 0, 255));

    // Now, compare that against the 'k' least similar vectors.
    int min_dist = kMaxDistanceSq;
    for (int j = 0; j < this->k_; j++)
    {
        const RGBVector<uint8_t> rgb(window[this->indices_[N - j - 1]],
                                     img.channels());
        const int sqdist = rgb.SquaredDistance(mean_rgb);
        min_dist = std::min(sqdist, min_dist);
    }

    // Output is the scaled magnitude between the two extracted vectors.
    const uint8_t value = 255*std::sqrt(static_cast<double>(min_dist))/kMaxDistance;
    return RGBVector<uint8_t>(value, value, value);
}

}} // namespace chromavec::internal
