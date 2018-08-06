#include "roi.h"

#include <algorithm>
#include <stdexcept>

namespace chromavec { namespace internal {

ROI::ROI(const cv::Mat &img, const int x, const int y, const int width)
    : ROI(img, x, y, width, width)
{
    // do nothing
}

ROI::ROI(const cv::Mat &img, const int x, const int y,
         const int width, const int height)
    : img_(img),
      x_(x),
      y_(y),
      x_start_(std::clamp(x - width/2, 0, img.cols-1)),
      x_end_(std::clamp(x + width/2, 0, img.cols-1)),
      y_start_(std::clamp(y - height/2, 0, img.rows-1)),
      y_end_(std::clamp(y + height/2, 0, img.rows-1))
{
    if (x < 0 || x >= img.cols)
        throw std::runtime_error("'x' cannot be outside of the image.");
    if (y < 0 || y >= img.rows)
        throw std::runtime_error("'y' cannot be outside of the image.");
}

const int ROI::Width() const
{
    return this->x_end_ - this->x_start_ + 1;
}

const int ROI::Height() const
{
    return this->y_end_ - this->y_start_ + 1;
}

const uint8_t *ROI::operator[](const int i) const
{
    const int x = i % this->Width() + this->x_start_;
    const int y = i / this->Width() + this->y_start_;
    return &this->img_.ptr<uint8_t>(y)[this->img_.channels()*x];
}

const uint8_t *ROI::operator()(const int x, const int y) const
{
    return &this->img_.ptr<uint8_t>(y + y_start_)[this->img_.channels()*(x + this->x_start_)];
}

}} // namespace chromavec::internal
