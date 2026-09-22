#include <errno.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* Ищет левый (direction = -1) или правый (direction = 1)
   корень около center = 2 * pi * k делением отрезка пополам */
static double find_root(double center, int direction, double eps)
{
    /* Ищем x = center + direction * t, 0 < t < pi / 2 */
    double left = 0.0;
    double right = acos(-1.0) / 2.0;

    for (;;) {
        double midpoint = left + (right - left) / 2.0;
        double root = center + direction * midpoint;

        /* Нижняя оценка для |x| нужна для контроля относительной ошибки */
        double min_absolute_root =
            fmin(fabs(center + direction * left),
                 fabs(center + direction * right));

        if ((right - left) / 2.0 <= eps * min_absolute_root)
            return root;

        /* Упёрлись в точность double */
        if (midpoint == left || midpoint == right)
            return NAN;

        /* ln(cos t) = ln(1 - 2 * sin^2(t / 2));
           log1p точнее при малых t */
        double sine = sin(midpoint / 2.0);
        double value = log1p(-2.0 * sine * sine) + exp(-root);

        if (!isfinite(value))
            return NAN;

        /* В левом конце значение положительно, в правом отрицательно */
        if (value > 0.0)
            left = midpoint;
        else
            right = midpoint;
    }
}

int main(int argc, char *argv[])
{
    /* Единственный параметр программы - требуемая относительная точность */
    if (argc != 2) {
        fprintf(stderr, "Usage: %s relative_eps\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *end;
    errno = 0;
    double eps = strtod(argv[1], &end);

    /* Проверяем, что аргумент целиком является допустимым конечным числом. */
    if (errno != 0 || end == argv[1] || *end != '\0' || !isfinite(eps) || eps < 100.0 * DBL_EPSILON || eps >= 1.0) {
        fprintf(stderr, "relative_eps must be a number in [%.17g, 1).\n", 100.0 * DBL_EPSILON);
        return EXIT_FAILURE;
    }

    const double pi = acos(-1.0);
    int period;
    double tail_error;

    printf("Relative eps: %.6g\n", eps);
    printf("  k                 left root                right root\n");

    for (period = 0; ; ++period) {
        double center = 2.0 * pi * period;

        /* Оценка относительной ошибки асимптотики
           x = 2 * pi * k +/- sqrt(2) * exp(-pi * k). */
        tail_error = 3.0 * exp(-center) / (center - pi / 2.0);

        /* Если асимптотика уже достаточно точна,
           численно считать последующие корни не нужно. */
        if (period > 0 && tail_error <= eps)
            break;

        double left_root =
            find_root(center, -1, eps);
        double right_root =
            find_root(center, 1, eps);

        if (!isfinite(left_root) || !isfinite(right_root)) {
            fprintf(stderr,
                    "Cannot reach the requested accuracy at k = %d.\n",
                    period);
            return EXIT_FAILURE;
        }

        printf("%3d  %24.17g  %24.17g\n",
               period, left_root, right_root);
    }

    /* Все оставшиеся корни при больших положительных k. */
    printf("\nFor every integer k >= %d:\n", period);
    puts("  left root  ~= 2 * pi * k - sqrt(2) * exp(-pi * k)");
    puts("  right root ~= 2 * pi * k + sqrt(2) * exp(-pi * k)");
    printf("  Relative approximation error < %.6e\n",
           tail_error);

    /* При k < 0 корни с огромной точностью близки
       к границам интервала определения. */
    puts("\nFor every integer k <= -1:");
    puts("  left root  ~= 2 * pi * k - pi/2");
    puts("  right root ~= 2 * pi * k + pi/2");
    printf("  Relative approximation error <= %.6e\n",
           exp(-exp(3.0 * pi / 2.0)) / 3.0);
    puts("  These endpoints are approximations, not exact roots.");

    return EXIT_SUCCESS;
}

