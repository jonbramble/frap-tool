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
Frap::Frap(char* pfile, char* cfile, bool _verbose){
	verbose = _verbose;
	prima = pfile;
	closed = cfile;	
	start_time = 10.0;
	npoints = 250;
}

Frap::~Frap(){
 if (s.selmade){
  gsl_matrix_free(exp_data);
  gsl_matrix_free(fitting_data); 
 }
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

void Frap::plplot_chart(char* _prefix){

	gsl_vector *x = gsl_vector_alloc(npoints);

	for(int a=0;a<npoints;a++)
	{
		gsl_vector_set(x,a,a*scaling_factor);
	}

	char const *f_name_gauss;   
	char const *f_name_lin;
	f_name_gauss = "gaussian_file.ps"; //names for files
	f_name_lin = "linear_file.ps";
	
	simple_chart = new Chart(f_name_gauss);	// create a new chart object
	simple_chart->plot(npoints,x,exp_data,fitting_data); 

	line_chart = new Chart(f_name_lin);
	line_chart->plot(imagefiles.size(),time_s,lambda,lambda_err, c1, c0);
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
	create_fit_data();
}

void Frap::print_data(){
	for(uint i=0; i<imagefiles.size(); i++){
		char* name = imagefiles[i].getfilename();
		printf("Filename: %s\tTimes: %f\tA: %f\tLambda: %f\tmu: %f\n",name,time_s[i],A[i],lambda[i],mu[i]);
	} 

	std::cout << "Diffusion Constant " << c1/2 << " μm2/s" << std::endl; 
}

void Frap::save_data_file(char* _prefix){

	double lambda_fit;
	char str_summary[80]; //string preparations for filenames
	//char str_fullset[80];
	//char str_linear_fit[80];
	
	strcpy(str_summary,_prefix);
	//strcpy(str_fullset,_prefix);
	//strcpy(str_linear_fit,_prefix);
	
	strcat(str_summary,"-summary.csv");
	//strcat(str_fullset,"-data-set.csv");
	//strcat(str_linear_fit,"-linear-fit.csv");

	ofstream data_file;

	std::cout << "Writing Datafiles...";
	
	data_file.open (str_summary);
	if (data_file.is_open())
	{
		data_file << "Filename,Time(s),A,A error,Lambda,Lambda error,mu, mu error,b,b error,Fitted Lambda\n";
		for(uint i=0; i<imagefiles.size(); i++){
			lambda_fit = c0+c1*time_s[i];
			data_file << imagefiles[i].getfilename() << "," << time_s[i] <<"," << A[i] <<"," << A_err[i] 
				<<"," << lambda[i] <<"," << lambda_err[i] <<"," 
				<< mu[i] <<"," << mu_err[i] <<"," << b[i] <<"," << b_err[i] << "," << lambda_fit << std::endl;
		} 
		data_file << "Diffusion Constant " << c1/2 << " μm2/s" << std::endl; 
	}
	data_file.close();

	//data_file.open (str_fullset);
	//data_file << "Writing this to a file.\n";
	//data_file.close();

	//data_file.open (str_linear_fit);
	//data_file << "Writing this to a file.\n";
	//data_file.close();

	std::cout << "...complete" << std::endl;
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
            pixlen = 150/942;       // microns/pixels
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
	
		float valimage, xk, yk, xstep, ystep;
		int x1,y1, x2, y2;
		int i=0;
	
		exp_data = gsl_matrix_alloc (imagefiles.size(), npoints);
		fitting_data = gsl_matrix_alloc (imagefiles.size(), npoints);
	
		x1 = s.getx1();
		x2 = s.getx2();
		y1 = s.gety1();
		y2 = s.gety2();

		std::cout << "(" << x1 << "," << y1 << ")"<< "(" << x2 << "," << y2 << ")" << std::endl;
		
		scaling_factor = (hypot(s.getxsize(),s.getysize())/npoints)/pixlen;

		//std::cout << s.getxsize() << "," << s.getysize() << std::endl;
		std::cout << "scaling factor " << scaling_factor << std::endl;

		if(s.getxsize()==0)
			xstep = 0.0;
		else 
			xstep = (float) s.getxsize()/npoints;
		if(s.getysize()==0)	
			ystep = 0.0;
		else 
			ystep = (float) s.getysize()/npoints;
		
		for(cimg_imageit=imagelist.begin(); cimg_imageit<imagelist.end(); cimg_imageit++)
		{
			for(uint k=0;k<npoints;k++){
				xk=x1+k*xstep; // calc x and y values
				yk=y1+k*ystep;
				valimage= cimg_imageit->cubic_atXY(xk,yk);   //cubic interpolation
				gsl_matrix_set (exp_data, i, k, valimage);
			}
		i++;
		}
	}
	else {
		std::cout << "no selection made" << std::endl;
	}
}

void Frap::dofitting(){
	double scaled_lambda;
	int ifilestotal = imagefiles.size();

	gsl_matrix *vdata = gsl_matrix_alloc(4,ifilestotal); 
	gsl_matrix *verr = gsl_matrix_alloc(4,ifilestotal); 
	gsl_vector *fit = gsl_vector_alloc(6); 

	Fitting::gaussfit(vdata,verr,get_exp_data(),verbose); // fits the data for all images

	for(uint i=0; i<ifilestotal; i++){
		A.push_back(gsl_matrix_get(vdata,0,i));  
		mu.push_back(gsl_matrix_get(vdata,1,i)); 
		scaled_lambda = gsl_matrix_get(vdata,2,i)*scaling_factor;
		lambda.push_back(scaled_lambda);  // put all the data into lambda vector
		b.push_back(gsl_matrix_get(vdata,3,i)); 

		A_err.push_back(gsl_matrix_get(verr,0,i));  
		mu_err.push_back(gsl_matrix_get(verr,1,i)); 
		scaled_lambda = gsl_matrix_get(verr,2,i)*scaling_factor;
		lambda_err.push_back(scaled_lambda);  // put all the data into lambda vector
		b_err.push_back(gsl_matrix_get(verr,3,i)); 
	}

	Fitting::linearfit(fit, &time_s[0],&lambda[0],ifilestotal, verbose); // needs lots of error handling --needs R factor cutoff

	c0 = gsl_vector_get(fit,0);
	c1 = gsl_vector_get(fit,1);
	cov00 = gsl_vector_get(fit,2);
	cov01 = gsl_vector_get(fit,3);
	cov11 = gsl_vector_get(fit,4);
	sumsq = gsl_vector_get(fit,5);
		
	gsl_matrix_free(vdata);
	gsl_matrix_free(verr);
	gsl_vector_free(fit);
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

void Frap::create_fit_data()// create curve data from fits
{
	double Yi, x_scaled;
	for(uint i=0; i<imagefiles.size(); i++){
		for(int x=0;x<npoints;x++){
			x_scaled = (x-mu[i])*scaling_factor;
			Yi = A[i]*exp((-1*pow(x_scaled,2))/(2*pow(lambda[i],2)))+b[i];
			gsl_matrix_set(fitting_data,i,x,Yi);
		}
	} 
}

}
