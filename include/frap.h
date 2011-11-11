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

	private:
		boost::thread m_Thread;
		char* prima;
		char* closed;
		double pixlen;			//<pixel scaling factor - from microscope
		double scaling_factor; 		//<pixel scaling factor - from selection length
		Selection s;

		gsl_matrix *data;		//<for output data

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

		vector<char*>::iterator fnameit;
};


#endif
