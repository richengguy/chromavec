/**
 * @file
 * @author Richard Rzeszutek
 * @date June 30, 2018
 */
#ifndef SRC_CHROMAVEC_UTILITIES_ROI_H_
#define SRC_CHROMAVEC_UTILITIES_ROI_H_

#include <opencv2/core.hpp>

namespace chromavec { namespace internal {

/**
 * @brief A region-of-interest handler for const access to cv::Mat objects.
 *
 * The ROI object takes care of all of the calculations necessary to access the
 * correct pixel in the source image.  The ROI is clamped to be within the image
 * bounds.  An ROI completely outside that range cannot be constructed.
 */
class ROI
{
public:

    /**
     * @brief Construct a symmetric ROI.
     * @param img
     *      image that the ROI is applied onto
     * @param x, y
     *      the centre of the window
     * @param width
     *      width and height of the window
     */
    ROI(const cv::Mat &img, const int x, const int y, const int width);

    /**
     * @brief Constructor
     * @param img
     *      image that the ROI is being applied onto
     * @param x, y
     *      the centre of the window
     * @param width
     *      width of the window
     * @param height
     *      height of the window
     */
    ROI(const cv::Mat &img, const int x, const int y,
        const int width, const int height);

    /**
     * @brief Returns the width of the window (includes clamping).
     */
    const int Width() const;

    /**
     * @brief Returns the height of the window (includes clamping).
     */
    const int Height() const;

    /**
     * @brief Select a pixel in the ROI using a linear index.
     * @param i
     *      linear index into the ROI
     * @return
     *      pointer to the pixel
     */
    const uint8_t *operator[](const int i) const;

    /**
     * @brief Select a pixel within the ROI.
     * @param x, y
     *      pixel coordinate in the ROI
     * @return
     *      pointer to the pixel
     */
    const uint8_t *operator()(const int x, const int j) const;

    // Default copy and assign
    ROI(const ROI &) = default;
    ROI(ROI &&) = default;
    ROI &operator=(const ROI &) = default;
    ROI &operator=(ROI &&) = default;

private:
    const cv::Mat &img_;
    const int x_, y_;
    const int x_start_, x_end_;
    const int y_start_, y_end_;
};

}} // namespace chromavec::internal

#endif // SRC_CHROMAVEC_UTILITIES_ROI_H_
