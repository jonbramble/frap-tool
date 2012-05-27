/*
 * fitting.h
 * Copyright (C) Jonathan Bramble 2011
 * 
frap-tool is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * frap-tool is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

namespace FrapTool {

struct data {
       size_t n;
       double *y;
       double *sigma;
};

class Fitting{
	public:
		static int gaussfit(gsl_matrix * vdata, gsl_matrix * verr, const gsl_matrix * m);
		static int linearfit(const double * x, const double * y, int n);
	private:
		static int gauss_f(const gsl_vector * x, void * data, gsl_vector * f);
		static int gauss_df(const gsl_vector * x, void * data, gsl_matrix * J);
		static int gauss_fdf(const gsl_vector * x, void * data, gsl_vector * f, gsl_matrix * J);

		static void print_state (size_t iter, gsl_multifit_fdfsolver * s);
	
};

}

#endif
