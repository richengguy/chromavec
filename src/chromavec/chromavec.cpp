#include "chromavec/chromavec.h"

#include <vector>

#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include "constants.h"

#include "filters/canny-edges.h"
#include "filters/minimum-vector-dispersion.h"
#include "filters/vmf.h"
#include "filters/vector-range.h"

namespace chromavec {

cv::Mat VectorMedianFilter(const cv::Mat &img, const int window)
{
    return internal::Filter<internal::VMFilter>(img, window);;
}

cv::Mat VectorRangeFilter(const cv::Mat &img, const int window)
{
    return internal::Filter<internal::VectorRangeFilter>(img, window);
}

cv::Mat MinimumVectorDispersionFilter(const cv::Mat &img, const int k,
                                      const int l, const int window)
{
    return internal::Filter<internal::MinVecDispersionFilter>(img, window,
                                                              k, l);
}

cv::Mat ColourVectorGradientFilter(const cv::Mat &img, const double sigma,
                                   const GradientMode mode)
{
    using internal::Filter;
    using internal::ColourGradient;
    using internal::GradientToHSV;

    cv::Mat filtered;
    if (sigma < 0.01)
    {
        img.copyTo(filtered);
    }
    else
    {
        cv::GaussianBlur(img, filtered, cv::Size(), sigma, 0,
                         cv::BORDER_REPLICATE);
    }

    cv::Mat out;

    switch (mode)
    {
        case kDirectOutput:
            out = Filter<ColourGradient>(filtered);
            break;
        case kMagnitudeOnly:
            {
                std::vector<cv::Mat> channels;
                cv::Mat temp = Filter<ColourGradient>(filtered);
                cv::split(temp, channels);
                channels[1].copyTo(out);
                out = 255*(out / internal::kMaxDistance);
            }
            break;
        case kToHSV:
            out = Filter<GradientToHSV>(Filter<ColourGradient>(filtered));
            cv::cvtColor(out, out, CV_HSV2BGR);
            break;
    }

    return out;
}

cv::Mat ColourCannyEdgeDetect(const cv::Mat &img, const double t1,
                              const double t2, const double sigma)
{
    using internal::Filter;
    using internal::ColourGradient;
    using internal::NonMaximumSupression;
    using internal::Threshold;
    using internal::ConnectedComponents;

    // Prefilter the image with a Gaussian kernel.
    cv::Mat filtered;
    if (sigma < 0.01)
    {
        img.copyTo(filtered);
    }
    else
    {
        cv::GaussianBlur(img, filtered, cv::Size(), sigma, 0,
                         cv::BORDER_REPLICATE);
    }

    // Perform Canny edge detection except using colour gradients.
    filtered = Filter<Threshold>(
        Filter<NonMaximumSupression>(
            Filter<ColourGradient>(filtered)
        ), t1, t2
    );

    // Run the connected components analysis, iterating until convergence.
    int i = 0;
    bool was_modified = true;
    while (was_modified)
    {
        was_modified = false;
        Filter<ConnectedComponents>(filtered,
                                    const_cast<const cv::Mat &>(filtered),
                                    was_modified);
        i++;
    }

    // Remove any remaining weak edges.
    return filtered > 127;
}

} // namespace chromavec
