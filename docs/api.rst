===
API
===

The API is accessed through the ``#include <chomravec/chromavec.h>`` header.
All of the utilities and functions reside within the ``chromavec::`` namespace.

Filtering Functions
===================

.. default-domain:: cpp
.. namespace:: chromavec

.. enum:: GradientMode

    Enums that define the set of output modes for the
    :func:`ColourVectorGradientFilter`.

    .. enum:: kDirectOutput

        Output a raw gradient image that is identical to what is passed internally
        within the chromavec library.

    .. enum:: kMagnitudeOnly

        Output the gradient magnitude image, scaled to be within 0 to 255.

    .. enum:: kToHSV

        Output the gradient as an RGB image with HSV colouring.  The gradient
        magnitude will be the value while the angle will be in the hue.


.. function:: cv::Mat VectorMedianFilter(const cv::Mat &img, const int window)

    Applies the Vector Median Filter onto an image.  This is a noise reduction
    filter that is quite similar to the standard median filter.

    :param img: input image
    :param window: filtering window size
    :return: filtered image


.. function:: cv::Mat VectorRangeFilter(const cv::Mat &img, const int window=5)

    Applies the Vector Range Filter onto an image.  The vector range filter is
    a type of edge detector that returns the distance between the most and
    least central RGB vector in a window.

    :param img: input image
    :param window: filtering window size
    :return: filter response map


.. function:: cv::Mat MinimumVectorDispersionFilter(const cv::Mat &img, \
                                                    const int k=3, \
                                                    const int l=4, \
                                                    const int window=5)

    The Minimum Vector Dispersion Filter is a combination between the VMF and
    VRF filters.  The output of the filter is the distance between the average
    ``k`` most-central vectors and the ``l`` least-central vectors.  This has a
    noise reduction affect on the edge map so that the edge response is less
    sensitive to noise in the image.

    :param img: input image
    :param k, l: the two parameters used to control between noise suppression \
                 and edge detection
    :param window: filtering window size
    :return: filter response map


.. function:: cv::Mat ColourVectorGradientFilter(const cv::Mat &img, \
                                                 const double sigma=0, \
                                                 const GradientMode mode=kToHSV)

    :param img: input image
    :param sigma: the sigma of a Gaussian pre-filter
    :param mode: the gradient output mode
    :return: colour gradient image


.. function:: cv::Mat ColourCannyEdgeDetect(const cv::Mat &img, \
                                            const double t1,\
                                            const double t2, \
                                            const double sigma=3.0)

    Perform Canny-style edge detection using colour gradients.

    :param img: input image
    :param t1, t2: the lower and upper Canny hysteresis thresholds
    :param sigma: pre-blurring amount

Miscellaneous
=============

The library version information is stored within a data structure, documented
below, that is generated at build time.  This is automatically included in the
main library header.

.. class:: Version

    The :class:`Version` struct provides version information about the chromavec
    library.  The values that it provides are filled in at build time and they
    follow `semver <https://semver.org/>`_ convention.

    .. member:: static constexpr int major

        Major version number.

    .. member:: static constexpr int minor

        Minor version number.

    .. member:: static constexpr int patch

        Patch version number.

    .. function:: static std::string ToString()

        Returns the version information as a string.  The string will be in
        ``<major>.<minor>.<patch>`` form.
