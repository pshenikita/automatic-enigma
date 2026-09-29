#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static const double EPS = 1e-12;

static double find_root(double center, int direction)
{
    double left = 0.;
    double right = acos(-1.) / 2.;

    for (;;) {
        double midpoint = left + (right - left) / 2.;
        double root = center + direction * midpoint;

        if ((right - left) / 2. <= EPS / 2.)
            return root;

        if (midpoint == left || midpoint == right)
            return NAN;

        double sine = sin(midpoint / 2.);
        double value = log1p(-2. * sine * sine) + exp(-root);

        if (!isfinite(value))
            return NAN;

        if (value > 0.)
            left = midpoint;
        else
            right = midpoint;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 1) {
        fprintf(stderr, "Usage: %s\n", argv[0]);
        return EXIT_FAILURE;
    }

    const double pi = acos(-1.);
    int period;
    double tail_error;

    printf("Absolute eps: %.6g\n", EPS);
    printf(" k        left root       right root\n");

    for (period = 0; ; ++period) {
        double center = 2. * pi * period;

        tail_error = 3. * exp(-center);

        if (period > 0 && tail_error <= EPS)
            break;

        double left_root = find_root(center, -1);
        double right_root = find_root(center, 1);

        if (!isfinite(left_root) || !isfinite(right_root)) {
            fprintf(stderr, "Cannot reach the requested accuracy at k = %d.\n",
                    period);
            return EXIT_FAILURE;
        }

        printf("%2d %16.12g %16.12g\n",
               period, left_root, right_root);
    }

    printf("\nFor every integer k >= %d:\n", period);
    puts("  left root  ~= 2 * pi * k - sqrt(2) * exp(-pi * k)");
    puts("  right root ~= 2 * pi * k + sqrt(2) * exp(-pi * k)");
    puts("  Absolute error bound: 3 * exp(-2 * pi * k)");
    printf("  Maximum bound in this tail is approximately %.6e\n", tail_error);

    puts("\nFor every integer k <= -1:");
    puts("  left root  ~= 2 * pi * k - pi / 2");
    puts("  right root ~= 2 * pi * k + pi / 2");
    puts("  Absolute error bound: (pi / 2) * exp(-exp(3 * pi / 2))");
    printf("  This bound is approximately %.6e\n", (pi / 2.) * exp(-exp(3. * pi / 2.)));
    puts("  These endpoints are approximations, not exact roots!");

    return EXIT_SUCCESS;
}

