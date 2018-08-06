#include <iostream>
#include <string>
#include <vector>

#include <CLI/CLI.hpp>
#include <CLI/Timer.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <chromavec/chromavec.h>

// Internal Functions
namespace {

struct Options
{
    std::vector<double> th;
    double sigma;
    bool verbose;
    std::string input, output;
    CLI::App app;

    Options()
        : th{10, 20},
          sigma(1.5),
          verbose(false),
          input(),
          output(),
          app("Canny-style Edge Detector")
    {
        app.add_option("-t, --thresholds", this->th,
                       "Canny detector upper and lower thresholds.", true)
           ->expected(2);

        app.add_option("-s, --sigma", this->sigma, "Gaussian filter sigma.", true);
        app.add_flag("-v, --verbose", this->verbose, "Show verbose output.");

        app.add_option("image", this->input, "Input image.")
           ->check(CLI::ExistingFile)
           ->required();
        app.add_option("edges", this->output, "Output edge map.")
           ->required();
    }
};

std::ostream &operator<<(std::ostream &os, const Options &opts)
{
    os << "Edge Detector:\n"
       << "  Input  - " << opts.input << "\n"
       << "  Output - " << opts.output << "\n"
       << "  Filter - threshold: [" << opts.th[0] << ", "
                                    << opts.th[1] << "]\n"
       << "               sigma: " << opts.sigma << "\n";
    return os;
}

} // end of anonymous namespace

int main(int nargs, char **args)
{

    Options options;
    CLI11_PARSE(options.app, nargs, args);

    if (options.verbose)
    {
        std::cout << "chomavec " << chromavec::Version::ToString() << "\n"
                  << options;
    }

    cv::Mat img = cv::imread(options.input);
    cv::Mat out;
    {
        CLI::Timer timer;
        out = chromavec::ColourCannyEdgeDetect(img,
                                               options.th[0],
                                               options.th[1],
                                               options.sigma);

        if (options.verbose)
            std::cout << timer.to_string() << "\n";
    }
    cv::imwrite(options.output, out);

    return 0;
}
