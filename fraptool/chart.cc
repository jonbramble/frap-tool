/*
 * chart.cc
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

#include "chart.h"

namespace FrapTool {

Chart::Chart(char const *f_name)
{
	pls = new plstream();
	char ver[80];

	pls->sdev( "ps" );   // the output device
	pls->sfnam( f_name );

	unsigned int col = 255;
	pls->scolbg(col,col,col);

	pls->setopt( "-geometry", "800x1024" );

	pls->init();
	pls->gver( ver );

	cout << "Plotting with PLplot version: " << ver << endl;

}

Chart::Chart()
{
	pls = new plstream();
	char ver[80];

	char *f_name;
	f_name = "a_file.eps";

	pls->sdev( "ps" );   // the output device
	pls->sfnam( f_name );

	unsigned int col = 255;
	pls->scolbg(col,col,col);

	pls->setopt( "-geometry", "800x1024" );

	pls->init();
	pls->gver( ver );

	cout << "Plotting with PLplot version: " << ver << endl;

}

void Chart::plot(int size, std::vector<double> &xp, std::vector<double> &yp, std::vector<double> &yp_err, double m, double c){

	int   i;
	double max_x, max_y;

	std::vector<double>::iterator result_min_x, result_max_x;
	std::vector<double>::iterator result_min_y, result_max_y;
	
    PLFLT *x = new PLFLT[size];
   	PLFLT *y = new PLFLT[size];
	PLFLT *y_fit = new PLFLT[size];
	PLFLT *y_err_hi = new PLFLT[size]; 
	PLFLT *y_err_lo = new PLFLT[size];

	for(i=0;i<size;i++){  // is there a better way with first value
		x[i]=xp[i];
		y[i]=yp[i];
		y_fit[i]=m*xp[i]+c;
		y_err_hi[i]=yp[i]+yp_err[i];
		y_err_lo[i]=yp[i]-yp_err[i];
	}

	result_min_x = std::min_element(xp.begin(), xp.end());
    result_max_x = std::max_element(xp.begin(), xp.end());
	result_min_y = std::min_element(yp.begin(), yp.end());
    result_max_y = std::max_element(yp.begin(), yp.end());
	
	max_x = (*result_max_x)*(1.2);	//sets the axis to be 20% larger than the max value
	max_y = (*result_max_y)*(1.2);

	pls->col0( 1 );
   	pls->env( 0, max_x, 0, max_y, 0, 1 );
    pls->col0( 2 );
    pls->lab( "time (s)", "lambda", "FRAP DATA" );

	pls->col0( 3 );
    pls->wid( 2 );
	pls->poin( size, x, y, 20);
    pls->erry( size, x, y_err_hi,y_err_lo);
    pls->wid( 1 );

	pls->col0( 3 );
    pls->wid( 2 );
    pls->line( size, x, y_fit );
    pls->wid( 1 );

    delete[] x;
    delete[] y;

}

void Chart::plot(int size, gsl_vector *xp, gsl_vector *yp){

	int i;
	
    PLFLT *x = new PLFLT[size];
   	PLFLT *y = new PLFLT[size];

	for(i=0;i<size;i++){  // is there a better way
		x[i]=gsl_vector_get(xp,i);
		y[i]=gsl_vector_get(yp,i);
	}
	
    pls->col0( 1 );
   	pls->env( 0, 100, 0, 25, 0, 1 );
    pls->col0( 2 );
    pls->lab( "x", "y", "FRAP DATA" );

   	 // Draw the line.

    pls->col0( 3 );
    pls->wid( 2 );
    pls->line( 100, x, y );
    pls->wid( 1 );

    delete[] x;
    delete[] y;
}

void Chart::plot(int size, gsl_vector *xp, gsl_matrix *exp_data, gsl_matrix *fitting_data){

	int   i;

	double max_x, max_y;

    PLFLT *x = new PLFLT[size];
   	PLFLT *y = new PLFLT[size];
	PLFLT *yf = new PLFLT[size];
	
	for(i=0;i<size;i++){  // is there a better way
	 x[i]=gsl_vector_get(xp,i);
	 y[i]=gsl_matrix_get(exp_data,0,i);
	 yf[i]=gsl_matrix_get(fitting_data,0,i);
	}

	max_x = gsl_vector_max(xp);
	max_y = 15000;

	pls->col0( 15 );
   	pls->env( 0, max_x, 0, max_y, 0, 1 );
    pls->col0( 15 );
    pls->lab( "Distance (μm)", "Inverted Intensity", "FRAP DATA" );

   	 // Draw the line.

    pls->col0( 14 );
    pls->wid( 2 );
    pls->poin( size, x, y, 20);
    pls->wid( 1 );
	pls->line( size, x, yf );

    delete[] x;
    delete[] y;
	delete[] yf;

}

Chart::~Chart()
{
	delete pls;
}

} // FrapTool
