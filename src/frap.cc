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

Frap::Frap(char* pfile, char* cfile){
	prima = pfile;
	closed = cfile;	
	start_time = 10.0;
}

Frap::~Frap(){
gsl_matrix_free(data);
}

gsl_matrix* Frap::getdata(){
	return data;
}

void Frap::plot_graph(){
	visu= new CImg<unsigned char>(s.getxsize(),s.getxsize(),1,3,255); //<a new display for the plot
	CImgDisplay draw_disp(*visu,"Data Plots");	//draw the display
	CImg<double> aplot(s.getxsize(),imagefiles.size(),1,1,0);
	double point;
	const unsigned char red[] = { 255,0,0 }, green[] = { 0,255,0 }, blue[] = { 0,0,255 };
	//get data
	for(uint j=0; j< imagefiles.size(); j++){
		for(uint i=0; i< s.getxsize(); i++){
			point = gsl_matrix_get(data,j,i);   // get data for first image
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


	/*for(uint j=0; j< imagefiles.size(); j++){
		for(uint i=0; i< s.getxsize(); i++){
			point = gsl_matrix_get(data,j,i);   // get data for first image
			aplot.set_linear_atXY(point,i,j);	// set data into array
		}
	}*/

	for(int a=0;a<s.getxsize();a++)
	{
		gsl_vector_set(x,a,a+1);
	}
	
	simple_chart = new Chart();	// create a new chart object
	//simple_chart->plot(10,x, y); 
	simple_chart->plot(s.getxsize(),x,data); 

}

void Frap::processdata()
{
	dosort();	// sort by time
	setimagelist();	// put the images in a list
	settimes();
	setpixlen();

	removebackground(); //could crop first to save time - at 60ms its neg
	getvectors();  // get the data and put it in a matrix
	dofitting(); // do the multid fitting on the gaussian profiles - TODO add baseline offset
		
	for(uint i=0; i<imagefiles.size(); i++){
		char* name = imagefiles[i].getfilename();
		printf("Filename: %s\tTimes: %f\tLambda: %f\n",name,time_s[i],lambda[i]);
	} 
}

void Frap::start()
{
	m_Thread = boost::thread(&Frap::doselection, this);
}

void Frap::join()
{
	m_Thread.join();
}

void Frap::dosort(){
	sort(imagefiles.begin(), imagefiles.end()); // uses overloaded < operator that compares the seconds from epoch
}

void Frap::setimagenames(vector<char*> ifiles){
	for(fnameit=ifiles.begin(); fnameit<ifiles.end(); fnameit++){
		Tiffile tifftmp(*fnameit);
		imagefiles.push_back(tifftmp);
	}
}  

void Frap::dofitting(){
	
	int ifilestotal = imagefiles.size();

	gsl_vector *vdata = gsl_vector_alloc(ifilestotal); // should use matrix for all data and errors
	gsl_vector *verr = gsl_vector_alloc(ifilestotal); // errors on lambda

	Fitting::gaussfit(vdata,verr,getdata()); // fits the data for all images

	for(uint i=0; i<ifilestotal; i++){
		lambda.push_back(gsl_vector_get(vdata,i));  // put all the data into lambda vector
	}

	Fitting::linearfit(&time_s[0],&lambda[0],ifilestotal); // needs lots of error handling

	gsl_vector_free(vdata);
	gsl_vector_free(verr);
}

void Frap::doselection(){
	s.selectline(closed);  //do selection on the closed image
}

void Frap::setimagelist(){
	for(imageit=imagefiles.begin(); imageit<imagefiles.end(); imageit++){
		CImg<float> image(imageit->getfilename());
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
	CImg<float> primaimg(prima);	
	for(uint i=0; i<imagefiles.size(); i++){
		imagelist[i]=primaimg-imagelist[i]; 	// need to check that these have been done	
	}		
}

void Frap::getvectors(){
	if(s.selmade){
	
		float m, c;
		int x1,x2;
		data = gsl_matrix_alloc (imagefiles.size(), s.getxsize());
	
		m = s.getm();	//get local versions
		c = s.getc();
		x1 = s.getx1();
		x2 = s.getx2();

		scaling_factor = hypot(s.getxsize(),s.getysize())/(s.getxsize()); // if pixlen is in um/pixel

		cout << scaling_factor << endl;

		for(uint i=0; i<imagefiles.size(); i++){
			float y, valimage;
			for(int xk = x1;xk<x2;xk++){			//needs error handling 
				y = round(m*xk+c);			//finds nearest pixel - could read more values?
				valimage = imagelist.atNXY(i,xk,(int)y);	// finds value at this pixel in each image
				gsl_matrix_set (data, i, xk-x1, valimage);	// put the data in the matrix - bug here
			}	
		}
	}
	else {
		cout << "no selection made" << endl;
		data = gsl_matrix_alloc (1, 1); // hack to avoid error on destructor
	}
}
