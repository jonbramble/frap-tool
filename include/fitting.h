#ifndef FITTING_H
#define FITTING_H

#include <stdlib.h>
#include <stdio.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>

#include <gsl/gsl_fit.h>


struct data {
       size_t n;
       double *y;
       double *sigma;
};

class Fitting{
	public:
		static int gaussfit(gsl_vector * vdata, gsl_vector * verr, const gsl_matrix * m);
		static int linearfit(const double * x, const double * y, int n);
	private:
		static int gauss_f(const gsl_vector * x, void * data, gsl_vector * f);
		static int gauss_df(const gsl_vector * x, void * data, gsl_matrix * J);
		static int gauss_fdf(const gsl_vector * x, void * data, gsl_vector * f, gsl_matrix * J);

		static void print_state (size_t iter, gsl_multifit_fdfsolver * s);
	
};

#endif
