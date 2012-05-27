/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

/*
 * frap.cc
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

#include "../include/frap.h" 

namespace FrapTool 
{

/*-- Constructor Destructors ---------------------------------------------------------------------------------*/
Frap::Frap(char* pfile, char* cfile){
	prima = pfile;
	closed = cfile;	
	start_time = 10.0;
}

Frap::~Frap(){
gsl_matrix_free(exp_data);
gsl_matrix_free(fitting_data);
}

/*-- Threading ---------------------------------------------------------------------------------*/
void Frap::start()
{
	m_Thread = boost::thread(&Frap::doselection, this);
}

void Frap::join()
{
	m_Thread.join();
}

/*-- Data Functions ----------------------------------------------------------------------------*/
gsl_matrix* Frap::get_exp_data(){
	return exp_data;
}

gsl_matrix* Frap::get_fitting_data(){
	return fitting_data;
}

/*-- Plotting ----------------------------------------------------------------------------------*/
void Frap::plot_graph(){
	visu= new cimg_library::CImg<unsigned char>(s.getxsize(),s.getxsize(),1,3,255); //<a new display for the plot
	cimg_library::CImgDisplay draw_disp(*visu,"Data Plots");	//draw the display
	cimg_library::CImg<double> aplot(s.getxsize(),imagefiles.size(),1,1,0);
	double point;
	const unsigned char red[] = { 255,0,0 }, green[] = { 0,255,0 }, blue[] = { 0,0,255 };
	//get data
	for(uint j=0; j< imagefiles.size(); j++){
		for(uint i=0; i< s.getxsize(); i++){
			point = gsl_matrix_get(exp_data,j,i);   // get data for first image
			aplot.set_linear_atXY(point,i,j);	// set data into array
		}
	}

	while (!draw_disp.is_closed()) {
      		draw_disp.wait();
		visu->draw_graph(aplot.get_crop(0,2,s.getxsize()-1,2),green,1,1,2,14000,100,0);  // image is fliped by swapping max and min
		visu->draw_graph(aplot.get_crop(0,1,s.getxsize()-1,1),blue,1,1,2,14000,100,0);  // 14000 is guess at max height, needs function
		visu->draw_graph(aplot.get_crop(0,0,s.getxsize()-1,0),red,1,1,2,14000,100,0).display(draw_disp); 
	}
}

void Frap::plplot_chart(){

	gsl_vector *x = gsl_vector_alloc(s.getxsize());

	for(int a=0;a<s.getxsize();a++)
	{
		gsl_vector_set(x,a,a+1);
	}
	
	simple_chart = new Chart();	// create a new chart object
	simple_chart->plot(s.getxsize(),x,exp_data,fitting_data); 

}

/*-- Processing ---------------------------------------------------------------------------------*/

void Frap::getfftransforms(){
	cimg_library::CImg<float> tmp;
	cimg_library::CImg<unsigned char> mag;
	//transform must be done centred on the image
	//it must also be scaled to 2^N pixels
	for(cimg_imageit=imagelist.begin(); cimg_imageit<imagelist.end(); cimg_imageit++){
		tmp = cimg_imageit->crop(s.getx1(),s.gety1(),s.getx2(),s.getx2()).resize(256,256).normalize(0,255); // use selection image numbers
		cimg_library::CImgList<float> tmp_fft = tmp.get_FFT();
		mag = ((tmp_fft[0].get_pow(2) + tmp_fft[1].get_pow(2)).sqrt()+1).log().normalize(0,255);
		//transforms.push_back(tmp_img.get_FFT()); 
	}

	cimg_library::CImgDisplay disp3(tmp,"Spatial Domain",0);
	while (!disp3.is_closed()) { 
      disp3.wait();
	}

	cimg_library::CImgDisplay disp2(mag,"Frequency Domain (Log)",0);
	while (!disp2.is_closed()) { 
      disp2.wait();
	}
	
}


void Frap::processdata()
{
	dosort();	// sort by time
	setimagelist();	// put the images in a list
	settimes();
	setpixlen();

	removebackground(); //could crop first to save time - at 60ms its neg
	getvectors();  // get the data and put it in a matrix
	dofitting(); // do the multid fitting on the gaussian profiles
	//getfftransforms(); //should do these on centred cropped images
		
	for(uint i=0; i<imagefiles.size(); i++){
		char* name = imagefiles[i].getfilename();
		printf("Filename: %s\tTimes: %f\tLambda: %f\tmu: %f\n",name,time_s[i],lambda[i],mu[i]);
	} 

	create_fitting_data();
}

void Frap::dosort(){
	sort(imagefiles.begin(), imagefiles.end()); // uses overloaded < operator that compares the seconds from epoch
}

void Frap::setimagelist(){
	for(imageit=imagefiles.begin(); imageit<imagefiles.end(); imageit++){
		cimg_library::CImg<float> image(imageit->getfilename());
		imagelist.push_back(image); 		
	}
}

void Frap::settimes(){
	double starttime = imagefiles.front().gettime()-start_time; //ten second start time
	for(imageit=imagefiles.begin(); imageit<imagefiles.end(); imageit++){
		time_s.push_back(imageit->gettime()-starttime);
	}
}

void Frap::setpixlen(){
	int binning = 1344/(imagefiles.front().getimagewidth());
	int objective = 40;

	switch( binning ) 
	{
		case 1:
			pixlen = 150/942;
		case 2: 
			pixlen = 150/471;
		case 4: 
			pixlen = 150/235.5;
	}	
}

void Frap::removebackground(){
	cimg_library::CImg<float> primaimg(prima);	
	for(uint i=0; i<imagefiles.size(); i++){
		imagelist[i]=primaimg-imagelist[i]; 	// need to check that these have been done	
	}		
}

void Frap::getvectors(){
	if(s.selmade){
	
		float m, c;
		int x1,x2;
		exp_data = gsl_matrix_alloc (imagefiles.size(), s.getxsize());
		fitting_data = gsl_matrix_alloc (imagefiles.size(), s.getxsize());
	
		m = s.getm();	//get local versions
		c = s.getc();
		x1 = s.getx1();
		x2 = s.getx2();

		scaling_factor = hypot(s.getxsize(),s.getysize())/(s.getxsize()); // if pixlen is in um/pixel

		cout << scaling_factor << endl;
        // this needs a rewrite
		for(uint i=0; i<imagefiles.size(); i++){
			float y, valimage;
			for(int xk = x1;xk<x2;xk++){			//needs error handling 
				y = round(m*xk+c);			//finds nearest pixel - could read more values?
				valimage = imagelist.atNXY(i,xk,(int)y);	// finds value at this pixel in each image
				gsl_matrix_set (exp_data, i, xk-x1, valimage);	// put the data in the matrix - bug here
			}	
		}
	}
	else {
		cout << "no selection made" << endl;
		exp_data = gsl_matrix_alloc (1, 1); // hack to avoid error on destructor
		fitting_data = gsl_matrix_alloc (1, 1); // hack to avoid error on destructor
	}
}

void Frap::dofitting(){
	
	int ifilestotal = imagefiles.size();

	gsl_matrix *vdata = gsl_matrix_alloc(4,ifilestotal); 
	gsl_matrix *verr = gsl_matrix_alloc(4,ifilestotal); 

	Fitting::gaussfit(vdata,verr,get_exp_data()); // fits the data for all images

	for(uint i=0; i<ifilestotal; i++){
		A.push_back(gsl_matrix_get(vdata,0,i));  
		mu.push_back(gsl_matrix_get(vdata,1,i));  
		lambda.push_back(gsl_matrix_get(vdata,2,i));  // put all the data into lambda vector
		b.push_back(gsl_matrix_get(vdata,3,i)); 
	}

	Fitting::linearfit(&time_s[0],&lambda[0],ifilestotal); // needs lots of error handling --needs R factor cutoff

	gsl_matrix_free(vdata);
	gsl_matrix_free(verr);
}


void Frap::doselection(){
	s.selectline(closed);  //do selection on the closed image
}

void Frap::setimagenames(vector<char*> ifiles){
	for(fnameit=ifiles.begin(); fnameit<ifiles.end(); fnameit++){
		Tiffile tifftmp(*fnameit);
		imagefiles.push_back(tifftmp);
	}
} 

void Frap::create_fitting_data()// create curve data from fits
{
	double Yi;
	for(uint i=0; i<imagefiles.size(); i++){
		for(int x=0;x<s.getxsize();x++){
			Yi = A[i]*exp((-1*pow(x-mu[i],2))/(2*pow(lambda[i],2)))+b[i];
			gsl_matrix_set(fitting_data,i,x,Yi);
		}
	} 
}

}
