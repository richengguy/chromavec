#include <iostream>
#include <string>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "../chromavec/utilities/filter.h"
#include "../chromavec/utilities/functions.h"

// Internal functions
namespace {

using namespace chromavec::internal;

struct GenerateImage : public OperatorBase<CV_8UC3, CV_8UC3>
{
    double angle;

    GenerateImage(const double theta)
        : angle(DegreesToRadians(theta))
    {
        // do nothing
    }

    RGBVector<uint8_t> operator()(const int x, const int y, const cv::Mat &img) const
    {
        const double cx = img.cols / 2;
        const double cy = img.rows / 2;

        auto get_sign = [&cx, &cy, &x, &y](const double angle) -> int
        {
            const double nx = std::cos(angle);
            const double ny = std::sin(angle);
            const double d = -nx*cx - ny*cy;
            const double prod = nx*x + ny*y + d;

            return prod > 0 ? 1 : -1;
        };

        RGBVector<uint8_t> rgb;
        rgb.red = 0;
        rgb.green = get_sign(this->angle) > 0 ? 129 : 0;
        rgb.blue = get_sign(this->angle) > 0 ? 0 : 254;

        return rgb;
    }
};

} // end of anonymous namespace

int main(int nargs, char **args)
{
    if (nargs != 3)
    {
        std::cout << "Usage: " << args[0] << " <angle> <output>\n";
        return 1;
    }

    cv::Mat img = cv::Mat::zeros(512, 512, CV_8UC3);
    chromavec::internal::Filter<GenerateImage>(img,
                                               const_cast<const cv::Mat &>(img),
                                               std::stod(args[1]));
    cv::imwrite(args[2], img);

    cv::cvtColor(img, img, CV_BGR2GRAY);
    cv::imwrite("greyscale.png", img);

    return 0;
}
