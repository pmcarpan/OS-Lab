#include <stdio.h>
#include <math.h>

#define MAXN 30
#define EPSILON 1E-6

int main(void) {
	int n, i;
	double a[MAXN], b[MAXN], c[MAXN], r, s, pr, ps, dr, ds;

	printf("Degree n = "); scanf("%d", &n);
	for (i = 0; i <= n; i++) {
		printf("Coeff of x^(%2d) = ", i); scanf("%lf", &a[i]);
	}

	printf("Enter initial approx for r, s: "); scanf("%lf %lf", &r, &s);

	do {
		b[n] = a[n]; 
		b[n-1] = a[n-1] + r * b[n];
		for (i = n - 2; i >= 0; i--) {
			b[i] = a[i] + r * b[i+1] + s * b[i+2];
		}

		c[n] = b[n]; 
		c[n-1] = b[n-1] + r * c[n];
		for (i = n - 2; i > 0; i--) {
			c[i] = b[i] + r * c[i+1] + s * c[i+2];
		}

		dr = (-b[1]*c[2] + b[0]*c[3]) / (c[2]*c[2] - c[1]*c[3]);
		ds = (-b[1]*c[1] + b[0]*c[2]) / (c[3]*c[1] - c[2]*c[2]);
		pr = r; ps = s;
		r += dr; s += ds;

		printf("r=%12lf, s=%12lf, dr=%12lf, ds=%12lf\n", r, s, dr, ds);
	} while ((fabs(dr) > EPSILON) || (fabs(ds) > EPSILON));

	return 0;
}

