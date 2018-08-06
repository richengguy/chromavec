#ifndef SRC_CHROMAVEC_VECTOR_H_
#define SRC_CHROMAVEC_VECTOR_H_

#include <cstdint>
#include <cmath>
#include <type_traits>

#include <opencv2/core.hpp>

namespace chromavec { namespace internal {

/**
 * @brief Define the type used for RGB difference vectors.
 */
template<typename T>
struct DiffType { typedef T type; };

/**
 * @brief Promote a uint8_t difference to int16_t.
 */
template<>
struct DiffType<uint8_t> { typedef int16_t type; };

/**
 * @brief Promote an int16_t difference into an int.
 */
template<>
struct DiffType<int16_t> { typedef int type; };

/**
 * Any integer type is automatically promoted to an integer.  Floating-point
 * types remain as the same type.
 *
 * @brief Define the type used for RGB vector magnitudes.
 */
template<typename T>
struct MagType
{
    typedef std::conditional_t<std::is_integral<T>::value, int, T> type;
};

/**
 * @brief Define an RGB vector
 */
template<typename T=uint8_t>
struct RGBVector
{
    using value_type = T;
    using diff_type = typename DiffType<T>::type;
    using mag_type = typename MagType<T>::type;

    T red;    ///< red component
    T green;  ///< green component
    T blue;   ///< blue component
    T unused; ///< unused and provided for 4-byte packing

    /**
     * @brief Default constructor.
     */
    RGBVector()
        : red(0),
          green(0),
          blue(0),
          unused(0)
    {
        // do nothing
    }

    /**
     * @brief Construct an RGBVector from a point to a pixel buffer element.
     * @param buffer
     *      buffer pointer
     * @param channels
     *      number of colour channels in the buffer
     */
    RGBVector(const T *buffer, const int channels)
        : RGBVector()
    {
        switch(channels)
        {
            case 4:
            case 3:
                this->blue = buffer[2];
            case 2:
                this->green = buffer[1];
            case 1:
                this->red = buffer[0];
        }
    }

    /**
     * @brief Convience constructor around OpenCV cv::Mat.
     * @param img
     *      the image
     * @param x, y
     *      the pixel coordinate
     * @warning
     *      The coordinate *are not* checked to see if they are valid.
     */
    RGBVector(const cv::Mat &img, const int x, const int y)
        : RGBVector(&img.ptr<T>(y)[x*img.channels()], img.channels())
    {
        // do nothing
    }

    /**
     * @brief Construct an RGB vector given the channel colours.
     * @param r, g, b
     *      the red, green and blue colour channels
     */
    RGBVector(const T r, const T g, const T b)
        : red(r),
          green(g),
          blue(b),
          unused(0)
    {
        // do nothing
    }

    RGBVector<diff_type> operator-(const RGBVector<T> &other) const
    {
        return RGBVector<diff_type>(
            static_cast<diff_type>(this->red) - static_cast<diff_type>(other.red),
            static_cast<diff_type>(this->green) - static_cast<diff_type>(other.green),
            static_cast<diff_type>(this->blue) - static_cast<diff_type>(other.blue)
        );
    }

    /**
     * @brief Compute the vector's squared magnitude.
     */
    mag_type SquaredMagnitude() const
    {
        const mag_type r = static_cast<mag_type>(this->red);
        const mag_type g = static_cast<mag_type>(this->green);
        const mag_type b = static_cast<mag_type>(this->blue);

        return r*r + g*g + b*b;
    }

    /**
     * @brief Compute the squared distance between two vectors.
     * @param other
     *      the other vector
     * @return
     *      the squared magnitude of `|this - other|`
     */
    mag_type SquaredDistance(const RGBVector<T> &other) const
    {
        return (*this - other).SquaredMagnitude();
    }
};

}} // namespace chromavec::internal

#endif // SRC_CHROMAVEC_VECTOR_H_
