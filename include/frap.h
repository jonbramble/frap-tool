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
#include <vector>
#include <algorithm>
#include <ctime>
#include "../include/tiffile.h"
#include "../include/fitting.h"
#include "../include/chart.h"

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>

#include <boost/thread.hpp>  

using namespace cimg_library;
using namespace std;

class Frap {
	public: 
		Frap(char* pfile, char* cfile);	
		~Frap(); //<must allocate mem to data before calling free
		
		void start();
		void join();
		void processdata();
		gsl_matrix* getdata();
		void setimagenames(vector<char*> ifiles);

		void plot_graph();
		void plplot_chart();

	private:
		boost::thread m_Thread;
		char* prima;
		char* closed;
		double pixlen;			//<pixel scaling factor - from microscope
		double scaling_factor; 		//<pixel scaling factor - from selection length
		double start_time;
		Selection s;

		gsl_matrix *data;		//<for output data
		
		Chart *simple_chart;		// make a chart

		CImg<unsigned char> *visu;	//<graph plot

		void dosort();	
		void doselection();
		void setimagelist();
		void settimes();
		void removebackground();
		void setpixlen();
		void getvectors();
		void dofitting();
		
		vector<Tiffile> imagefiles;	//<this vector contains all the information about each file
		vector<double> lambda;		//<this vector contains the lambda data
		vector<double> time_s; 		//<time data in seconds from the start point of experiment
		CImgList<float> imagelist;	//<this is a list of the image data	

		vector<char*>::iterator fnameit;	//iterators
		vector<Tiffile>::iterator imageit;
};


#endif
