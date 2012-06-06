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

#ifndef FRAP_H
#define FRAP_H

#include "CImg.h"
#include "unistd.h"
#include "selection.h"

#include <pthread.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>

#include <ctime>
#include <cstring>

#include "../include/tiffile.h"
#include "../include/fitting.h"
#include "../include/chart.h"

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>

namespace FrapTool {

class Frap {
	public: 
		Frap(char* pfile, char* cfile, bool verbose);	
		~Frap(); //<must allocate mem to data before calling free
		
		void processdata();
		void doselection();

		gsl_matrix* get_exp_data();
		gsl_matrix* get_fitting_data();
		void setimagenames(vector<std::string> ifiles);

		void plot_graph();
		void plplot_chart(char* _prefix);

		void save_data_file(char* _prefix);
		void print_data();

	private:
		char* prima;
		char* closed;
		bool verbose;
		int npoints;			 // number of points to use in selection interpolation
		double pixlen;			 //<pixel scaling factor - from microscope
		double scaling_factor; 	 //<pixel scaling factor - from selection length
		double start_time;
		Selection s;

		char * cstr;

		gsl_matrix *exp_data;		//<for output data
		gsl_matrix *fitting_data;	//<for output data
		
		Chart *simple_chart;	// make a chart
		Chart *line_chart;		// make a chart

		cimg_library::CImg<unsigned char> *visu;	//<graph plot

		void dosort();			//separate sub routines to merge later
		
		void setimagelist();
		void settimes();
		void removebackground();
		void setpixlen();
		void getvectors();
		void getfftransforms();
		void dofitting();
		void create_fit_data();
		
		std::vector<Tiffile> imagefiles;	//<this vector contains all the information about each file

		std::vector<double> A;			//<this vector contains the A data
		std::vector<double> lambda;		//<this vector contains the lambda data
		std::vector<double> lambda_2;		//<this vector contains the lambda data
		std::vector<double> mu;
		std::vector<double> b;
		std::vector<double> time_s; 	//<time data in seconds from the start point of experiment

		std::vector<double> A_err;		//<this vector contains the A errors
		std::vector<double> lambda_err;	//<this vector contains the lambda errors
		std::vector<double> lambda_err_2;		//<this vector contains the lambda data
		std::vector<double> mu_err;
		std::vector<double> b_err;

		double c0, c1, cov00, cov01, cov11, sumsq;  //<linear fit parameters
		
		cimg_library::CImgList<float> imagelist;	//<this is a list of the image data	
		cimg_library::CImgList<float> transforms;	//<this is a list of the fourier transformed data

		std::vector<std::string>::iterator fnameit;	//iterators
		std::vector<Tiffile>::iterator imageit;
		cimg_library::CImgList<float>::iterator cimg_imageit;
};

} // FrapTool

#endif
