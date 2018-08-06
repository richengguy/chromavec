#include <chromavec/version.h>

namespace chromavec {

std::string Version::ToString()
{
    return std::to_string(major) + "." +
           std::to_string(minor) + "." +
           std::to_string(patch);
}

} // namespace chromavec
