#include <functional>
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

/**
 * @brief CLI11 validator to check that a value is greater than some minimum.
 *
 * This is a modified version of the CLI::Range validator.
 */
struct MinValue : public CLI::Validator
{
    template<typename T>MinValue(const T value)
    {
        std::stringstream out;
        out << CLI::detail::type_name<T>() << " >= " << value;

        tname = out.str();
        func = [value](std::string input) -> std::string
        {
            T val;
            CLI::detail::lexical_cast(input, val);
            if (val < value)
                return "Value " + input + " must be larger than " + std::to_string(value);

            return std::string();
        };
    }
};

/**
 * @brief Define the application options.
 */
struct Options
{
    std::string input, output;
    bool verbose;
    CLI::App app;

    /**
     * @brief Constructor
     * @param desc
     *      application description
     */
    Options(const std::string &desc)
        : input(),
          output(),
          verbose(false),
          app(desc)
    {
        app.require_subcommand(1);
        app.add_flag("-v, --verbose", this->verbose, "Verbose output.");
    }

    /**
     * @brief Add a subcommand while also registering the appropriate arguments.
     * @param cmd
     *      name of the subcommand
     * @param desc
     *      subcommand's description
     */
    CLI::App *AddSubcommand(const std::string &cmd, const std::string &desc)
    {
        CLI::App *subcmd = this->app.add_subcommand(cmd, desc);
        subcmd->add_option("input", this->input, "Input image")
              ->check(CLI::ExistingFile)
              ->required();
        subcmd->add_option("output", this->output, "Output image")
              ->required();
        return subcmd;
    }
};

/**
 * @brief Used to store the MVDF options.
 */
struct MVDFOptions
{
    int window;
    int k;
    int l;

    MVDFOptions()
        : window(5),
          k(4),
          l(3)
    {
        // do nothing
    }
};

/**
 * @brief Wraps a filter call to help with the CLI11 callbacks.
 */
template<typename Filter, typename ...Args>
void RunFilter(Filter filter, const Options &options, const std::string &name,
               Args &&...args)
{
    if (options.verbose)
        std::cout << "Filter: " << name << "\n";

    cv::Mat img = cv::imread(options.input);
    cv::Mat out;
    {
        CLI::Timer timer;
        out = filter(img, std::forward<Args>(args)...);
        if (options.verbose)
            std::cout << timer.to_string() << "\n";
    }
    cv::imwrite(options.output, out);
}

} // end of anonymous namespace

int main(int nargs, char **args)
{
    // Top-level CLI Options
    Options options("Filter images using vector-order statistic filters.");
    options.app.callback([&options]() {
        if (options.verbose)
            std::cout << "chomavec " << chromavec::Version::ToString() << "\n";
    });

    // Filter options to avoid losing values when leaving scope.  This can be
    // brought out into separate files.

    // Common Options
    int window = 5;

    // MVDF Options
    int k = 4;
    int l = 3;

    // Vector Gradient Options
    double sigma = 0.0;
    bool just_mag = false;

    // Define Minimum Vector Dispersion Filter.
    {
        auto mvdf = options.AddSubcommand("mvdf",
                                          "Minimum Vector Dispersion Filter");

        mvdf->add_option("-w, --window", window,
                         "Size of the NxN filter window.", true)
            ->check(MinValue(3));
        mvdf->add_option("-k", k, "Controls the sensitivity to edges.", true)
            ->check(MinValue(1));
        mvdf->add_option("-l", l, "Controls the amount of pre-smoothing.", true)
            ->check(MinValue(1));

        mvdf->callback([&]()
        {
            std::cout << "w: " << window << " k: " << k << " l: " << l << "\n";
            RunFilter(chromavec::MinimumVectorDispersionFilter, options,
                      "Minimum Vector Dispersion", k, l, window);
        });
    }

    // Define the Vector Range Filter.
    {
        auto vr = options.AddSubcommand("vector-range", "Vector Range Filter");
        vr->add_option("-w, --window", window, "Size of the NxN filter window.")
          ->check(MinValue(3));

        vr->callback([&]()
        {
            RunFilter(chromavec::VectorRangeFilter, options, "Vector Range",
                      window);
        });
    }

    // Define the Vector Median Filter.
    {
        auto vecmed = options.AddSubcommand("vector-median",
                                            "Vector Median Filter");
        vecmed->add_option("-w, --window", window,
                           "Size of the NxN filter window.")
              ->check(MinValue(3));

        vecmed->callback([&]()
        {
            RunFilter(chromavec::VectorMedianFilter, options, "Vector Median",
                      window);
        });
    }

    // Define the Vector Gradient Filter.
    {
        auto vecgrad = options.AddSubcommand("vector-gradient",
                                             "Vector Colour Gradient Filter");
        vecgrad->add_option("-s, --sigma", sigma, "Gaussian pre-filter sigma.",
                            true);
        vecgrad->add_flag("-m, --only-magnitude", just_mag,
                          "Only output the gradient magnitudes.");

        vecgrad->callback([&]()
        {
            std::cout << "sigma: " << sigma << "\n";
            const chromavec::GradientMode mode =
                just_mag ? chromavec::kMagnitudeOnly : chromavec::kToHSV;
            RunFilter(chromavec::ColourVectorGradientFilter, options,
                      "Vector Colour Gradient", sigma, mode);
        });
    }

    // Parsing will call the various callback that do the actual work.
    CLI11_PARSE(options.app, nargs, args);

    return 0;
}
