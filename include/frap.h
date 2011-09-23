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


using namespace cimg_library;
using namespace std;

class Frap {
	public: 
		Frap(char* pfile, char* cfile, vector<char*> ifiles);	
		~Frap(){gsl_matrix_free(data);} //must allocate mem to data before calling free
		gsl_matrix* getdata(){return data;};

	private:
		Selection s;
		void dosort();
		void doselection();
		void setimagenames(vector<char*> ifiles);
		void setimagelist();
		void settimes();
		void removebackground();
		void setpixlen();
		void getvectors();
		void dofitting();
		char* prima;
		char* closed;
		vector<Tiffile> imagefiles;	//this vector contains all the information about each file
		vector<double> lambda;		//this vector contains the lambda data
		vector<double> time_s; 		//time data in seconds from the start point of experiment
		CImgList<float> imagelist;	//this is a list of the image data
		double pixlen;
		double scaling_factor;
		gsl_matrix *data;		//for output data
};


#endif
