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

#include "../include/chart.h"

Chart::Chart()
{
	pls = new plstream();
	char ver[80];
	f_name  = "test_file.ps";

	pls->sdev( "ps" );   // the output device
	pls->sfnam( f_name );
	pls->init();
	pls->gver( ver );

	cout << "Plotting with PLplot version: " << ver << endl;

}

void Chart::plot(int size, gsl_vector *xp, gsl_vector *yp){

	int   i;
    	PLFLT *x = new PLFLT[size];
   	PLFLT *y = new PLFLT[size];

	for(i=0;i<size;i++){  // is there a better way
		x[i]=gsl_vector_get(xp,i);
		y[i]=gsl_vector_get(yp,i);
	}

    	// Set up the viewport and window using PLENV. The range in X is -2.0 to
    	// 10.0, and the range in Y is -0.4 to 2.0. The axes are scaled
    	// separately (just = 0), and we draw a box with axes (axis = 1).
		// edited

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

void Chart::plot(int size, gsl_vector *xp, gsl_matrix *data){

	int   i;
    	PLFLT *x = new PLFLT[size];
   	PLFLT *y = new PLFLT[size];
	
	for(i=0;i<size;i++){  // is there a better way
		x[i]=gsl_vector_get(xp,i);
		y[i]=gsl_matrix_get(data,0,i);
	}


	pls->col0( 1 );
   	pls->env( 0, size, 0, 15000, 0, 1 );
    	pls->col0( 2 );
    	pls->lab( "x", "y", "FRAP DATA" );

   	 // Draw the line.

    	pls->col0( 3 );
    	pls->wid( 2 );
    	pls->line( size, x, y );
    	pls->wid( 1 );

    	delete[] x;
    	delete[] y;

}




Chart::~Chart()
{
	delete pls;

}
