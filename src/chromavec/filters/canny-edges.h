/**
 * @file
 * @author Richard Rzeszutek
 * @date July 15, 2018
 */
#ifndef SRC_CHROMAVEC_CANNY_EDGES_H_
#define SRC_CHROMAVEC_CANNY_EDGES_H_

#include <array>

#include "constants.h"
#include "utilities/filter.h"
#include "utilities/functions.h"
#include "utilities/rgbvector.h"

namespace chromavec { namespace internal {

constexpr std::array<int, 4> kAngles{
    0,
    90,
    45,
    135
};

template<typename T, int dx, int dy>
int CalcRGBDelta(const cv::Mat &img, const int x, const int y)
{
    const auto c1 = ClampCoordinate(img, x + dx, y + dy);
    const auto c2 = ClampCoordinate(img, x - dx, y - dy);

    const RGBVector<T> p1(img, c1.first, c1.second);
    const RGBVector<T> p2(img, c2.first, c2.second);

    return std::sqrt(p1.SquaredDistance(p2));
}

/**
 * A colour gradient is an indication of the amount of colour change in a
 * particular direction.  Because the gradient is computed as a vector norm,
 * there is no preferred direction *across* an edge; both are equally valid.
 * However, it is guaranteed that the vector will always be perpendicular to
 * the actual edge.
 *
 * @brief Compute an image's colour gradients.
 */
struct ColourGradient : public OperatorBase<CV_8UC3, CV_32SC3>
{
    RGBVector<int> operator()(const int x, const int y, const cv::Mat &img) const
    {
        typedef typename OpenCVTypeInfo<ColourGradient::input_type>::type type;

        // This performs a series of comparisons around the central pixel.  Any
        // pixels on the edge are assumed to be replicated.  The scan looks
        // something like this:
        //
        //     0-degrees
        //     o o o
        //     - x +
        //     o o o
        //
        //     90-degrees
        //     o + o
        //     o x o
        //     o - o
        //
        //     45-degrees
        //     o o +
        //     o x o
        //     - o o
        //
        //     135-degrees
        //     - o o
        //     o x o
        //     o o +
        //
        // The cardinal directions are checked *first* so that the expected
        // gradient responses are returned.

        const std::array<int, 4> sqdist{
            CalcRGBDelta<type, 1, 0>(img, x, y),  //   0-degrees
            CalcRGBDelta<type, 0, 1>(img, x, y),  //  90-degrees
            CalcRGBDelta<type, 1, 1>(img, x, y),  //  45-degrees
            CalcRGBDelta<type, 1,-1>(img, x, y)   // 135-degrees
        };

        // Find the maximum gradient of the four that were tested.
        int max_ind = 0;
        int max_grad = 0;

        for (size_t i = 0; i < sqdist.size(); i++)
        {
            if (sqdist[i] > max_grad)
            {
                max_grad = sqdist[i];
                max_ind = i;
            }
        }

        // Since the non-maximum suppression is already operating on the polar
        // values (i.e. magnitude and angle), these can be stored directly.
        const int theta = kAngles[max_ind];
        const int rho = max_grad;

        return RGBVector<int>(theta, rho, 0);
    }
};

/**
 * @brief Convert a gradient image into HSV.
 */
struct GradientToHSV : public OperatorBase<CV_32SC3, CV_8UC3>
{
    RGBVector<uint8_t> operator()(const int x, const int y, const cv::Mat &img) const
    {
        const RGBVector<int> rgb(img, x, y);
        return RGBVector<uint8_t>(
            255*(RadiansToDegrees(rgb.red)/360.0),
            255,
            255*(static_cast<double>(rgb.green) / kMaxDistance)
        );
    }
};

/**
 * @brief Perform Canny-style non-maximum suppresion on a gradient image.
 */
struct NonMaximumSupression : public OperatorBase<CV_32SC3, CV_32SC1>
{
    RGBVector<int> operator()(const int x, const int y, const cv::Mat &img) const
    {
        const RGBVector<int> rgb(img, x, y);

        const int theta = rgb.red;
        const int current_mag = rgb.green;

        // Compute the sampling direction.  This is determined by the angle of
        // the gradient at the current pixel location.
        int dx = 0, dy = 0;
        if (theta < 22.5)
        {
            dx = 1;
            dy = 0;
        }
        else if (theta >= 22.5 && theta < 67.5)
        {
            dx = 1;
            dy = 1;
        }
        else if (theta >= 67.5 && theta < 112.5)
        {
            dx = 0;
            dy = 1;
        }
        else if (theta >= 112.5)
        {
            dx =  1;
            dy = -1;
        }

        // Sample the pixel values based on the angle, clamping if outside of
        // the image if necessary.
        auto get_magnitude = [&img](const int x, const int y) -> int
        {
            const auto c = ClampCoordinate(img, x, y);
            return RGBVector<int>(img, c.first, c.second).green;
        };

        const int m1 = get_magnitude(x + dx, y + dy);
        const int m2 = get_magnitude(x - dx, y - dy);

        // Check to see if the pixel is a maxima on either side of the gradient.
        const bool is_max = m1 <= current_mag && m2 <= current_mag;
        const int response = is_max ? current_mag : 0;
        return RGBVector<int>(response, response, response);
    }
};

/**
 * @brief Threshold a magnitude image using a double threshold.
 */
struct Threshold : OperatorBase<CV_32SC1, CV_8UC1>
{
    float min_th;
    float max_th;

    /**
     * @brief Constructor
     * @param min
     *      lower threshold
     * @param max
     *      upper threshold
     */
    Threshold(const float min, const float max)
        : min_th(min),
          max_th(max)
    {
        // do nothing
    }

    RGBVector<uint8_t> operator()(const int x, const int y, const cv::Mat &img) const
    {
        const RGBVector<int> rgb(img, x, y);

        RGBVector<uint8_t> out;
        if (rgb.red > max_th)
            out = RGBVector<uint8_t>(255, 255, 255);
        else if (rgb.red > min_th)
            out = RGBVector<uint8_t>(127, 127, 127);
        else
            out = RGBVector<uint8_t>(0, 0, 0);

        return out;
    }
};

/**
 * This uses the output of the Canny double-thresholding to produce the final
 * set of edges.  This is expected to be performed iteratively until
 * convergence.
 *
 * @brief Perform connected-component analysis using double thresholds.
 */
struct ConnectedComponents : OperatorBase<CV_8UC1, CV_8UC1>
{
    bool &was_modified;

    /**
     * @brief Constructor
     * @param mod
     *      a reference to an external boolean variable that will be used to
     *      indicate whether or not the image was modified
     */
    ConnectedComponents(bool &mod)
        : was_modified(mod)
    {
        // do nothing
    }

    RGBVector<uint8_t> operator()(const int x, const int y, const cv::Mat &img) const
    {
        const RGBVector<uint8_t> rgb(img, x, y);

        // This is not an edge pixel, so nothing to do.
        if (rgb.red < 127)
            return rgb;

        // This is a strong edge, so also, nothing to do.
        if (rgb.red == 255)
            return rgb;

        // This is a weak edge, so need to look at its neighbours to see if one
        // of them is a strong edge.  If it's a strong edge, mark it as a strong
        // edge.  Otherwise, do nothing.
        for (int dy = -1; dy <= 1; dy++)
            for (int dx = -1; dx <= 1; dx++)
            {
                const auto coord = ClampCoordinate(img, x+dx, y+dy);
                const auto value = RGBVector<uint8_t>(img,
                                                      coord.first,
                                                      coord.second).red;

                if (value == 255)
                {
                    this->was_modified = true;
                    return RGBVector<uint8_t>(255, 255, 255);
                }
            }

        return rgb;
    }
};

}} // namespace chromavec::internal

#endif // SRC_CHROMAVEC_CANNY_EDGES_H_
