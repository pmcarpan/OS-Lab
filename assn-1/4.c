#include <stdio.h>

#define EPS 1E-6

double f(double x, double y) {
	return y - x;
}

int main(void) {
	int iter = 0;
	double x, y, h, x0, y0, x_final, k1, k2;

	printf("Enter x0, y0, x, h: "); scanf("%lf %lf %lf %lf", &x0, &y0, &x_final, &h);

	x = x0; y = y0;

	printf("%12s%12s%12s%12s%12s\n", "Iter. No.", "x", "k1", "k2", "y");
	printf("------------------------------------------------------------\n");
	printf("%12d%12lf%12s%12s%12lf\n", iter, x, "", "", y);

	while (x < x_final - EPS) {
		k1 = h * f(x, y);
		k2 = h * f(x + h, y + k1);
		y = y + 0.5 * (k1 + k2);

		x += h;
		printf("%12d%12lf%12lf%12lf%12lf\n", ++iter, x, k1, k2, y);
	}

	return 0;
}
