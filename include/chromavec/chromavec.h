/**
 * @file
 * @brief Colour filtering with vector order statistics.
 * @author Richard Rzeszutek
 * @date June 25, 2018
 */
#ifndef CHROMAVEC_CHROMAVEC_H_
#define CHROMAVEC_CHROMAVEC_H_

#include <opencv2/core.hpp>

#include <chromavec/version.h>

namespace chromavec {

/**
 * @brief Gradient output modes.
 */
enum GradientMode
{
    kDirectOutput,  ///< Output the raw gradient image.
    kMagnitudeOnly, ///< Output the magnitude image.
    kToHSV          ///< Output the gradient as an RGB image with HSV colouring.
};

/**
 * @brief The Vector Median filter.
 * @param img
 *      input image
 * @param window
 *      filtering window size
 * @return
 *      filtered image
 */
cv::Mat VectorMedianFilter(const cv::Mat &img, const int window=5);

/**
 * @brief The Vector Range filter.
 * @param img
 *      input image
 * @param window
 *      filtering window size
 * @return
 *      output edge map
 */
cv::Mat VectorRangeFilter(const cv::Mat &img, const int window=5);

/**
 * @brief The Minimum Vector Dispersion filter.
 * @param img
 *      input image
 * @param k, l
 *      the two parameters used to control between noise suppression and edge
 *      detection
 * @param window
 *      filtering window size
 * @return
 *      output edge map
 */
cv::Mat MinimumVectorDispersionFilter(const cv::Mat &img, const int k=3,
                                      const int l=4, const int window=5);

/**
 * @brief Compute colour edge gradients.
 * @param img
 *      input image
 * @param sigma
 *      the sigma of a Gaussian pre-filter
 * @param mode
 *      the gradient output mode
 * @return
 *      colour gradient image
 */
cv::Mat ColourVectorGradientFilter(const cv::Mat &img, const double sigma=0,
                                   const GradientMode mode=kToHSV);

/**
 * @brief Perform Canny-style edge detection using colour gradients.
 * @param img
 *      input image
 * @param t1, t2
 *      the lower and upper Canny hysteresis thresholds
 * @param sigma
 *      pre-blurring amount
 */
cv::Mat ColourCannyEdgeDetect(const cv::Mat &img,
                              const double t1, const double t2,
                              const double sigma=3.0);

} // namespace chromavec

#endif // CHROMAVEC_CHROMAVEC_H_
