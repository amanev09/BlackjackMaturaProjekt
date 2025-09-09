#pragma once
#include <cmath>
#include <cstdint>

namespace bj {

    struct OnlineStats {
        std::uint64_t n = 0;
        double mean = 0.0; // EV estimate, streaming data
        double M2 = 0.0; // sum of squared deviation

        // Add one observation x (unit return for a resulting hand)
        inline void add(double x) {
            if (!std::isfinite(x)) return;
            ++n;
            double delta = x - mean;
            mean += delta / static_cast<double>(n);
            double delta2 = x - mean;
            M2 += delta * delta2;
        }

        inline std::uint64_t count() const { return n; }

        // Unbiased sample variance and SD (n>=2)
        inline double variance() const {
            return (n > 1) ? (M2 / static_cast<double>(n - 1)) : 0.0;
        }
        inline double sd() const { return std::sqrt(variance()); }

        // Standard error of the mean (precision of EV estimate)
        inline double sem() const {
            return (n > 1) ? (std::sqrt(variance() / static_cast<double>(n))) : 0.0;
        }
    };

    struct WeightedOnlineStats {
        long double wsum = 0.0L; // sum of weights
        long double weightedMean = 0.0L; // weighted mean of x
        long double M2 = 0.0L; // weighted sum of squared deviations
        long double w2sum = 0.0L; // sum of w^2 (for effective sample size)

        inline void add(double x, double w) {
            if (!std::isfinite(x) || !std::isfinite(w) || w <= 0.0) return;
            long double wl = (long double)w;
            long double wnew = wsum + wl;
            long double delta = (long double)x - weightedMean;
            weightedMean += (wl / wnew) * delta;
            M2 += (wsum * wl / wnew) * delta * delta;
            wsum = wnew;
            w2sum += wl * wl;
        }

        inline double mean() const { return (double)weightedMean; }
        inline double variance() const { return (wsum > 0.0L) ? (double)(M2 / wsum) : 0.0; }
        inline double sd() const { return std::sqrt(variance()); }

        // effective sample size
        inline double n_eff() const { return (w2sum > 0.0L) ? (double)((wsum * wsum) / w2sum) : 0.0; }

        // standard error of the weighted mean
        inline double sem() const {
            double ne = n_eff();
            return (ne > 0.0) ? std::sqrt(variance() / ne) : 0.0;
        }
    };

} 