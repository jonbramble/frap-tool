/*
 * chart.h
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

#ifndef CHART_H
#define CHART_H

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include <plplot/plstream.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>

#include <ctime>
#include <cstring>

#ifdef PL_USE_NAMESPACE
using namespace std;
#endif

namespace FrapTool {

class Chart 
{
	public:

		Chart();
		Chart(char const *f_name);
		~Chart();
		void plot(int size, std::vector<double> &xp, std::vector<double> &yp, std::vector<double> &yp_err, double m, double c);
		void plot(int size, gsl_vector *xp, gsl_vector *yp);
		void plot(int size, gsl_vector *xp, gsl_matrix *exp_data, gsl_matrix *fitting_data);
		
	private:
		plstream *pls;

};


}

#endif
