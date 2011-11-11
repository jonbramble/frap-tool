#include "../include/frap.h" 

Frap::Frap(char* pfile, char* cfile)
{
	prima = pfile;
	closed = cfile;	
}

Frap::~Frap()
{
gsl_matrix_free(data);
}

gsl_matrix* Frap::getdata(){
	return data;
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
	for(uint i=0; i<imagefiles.size(); i++){
		CImg<float> image(imagefiles[i].getfilename());
		imagelist.push_back(image); 		
	}
}

void Frap::settimes(){
	double starttime = imagefiles[0].gettime()-10.0; //ten second start time
	for(uint i=0; i<imagefiles.size(); i++){
		time_s.push_back(imagefiles[i].gettime()-starttime);		
	}
}

void Frap::setpixlen(){
	int binning = 1344/(imagefiles[0].getimagewidth());
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
	//std::cout << pixlen << std::endl;
		
}

void Frap::removebackground(){
	clock_t c0, c1;
	c0 = clock();

	CImg<float> primaimg(prima);	
	for(uint i=0; i<imagefiles.size(); i++){
		imagelist(i)=primaimg-imagelist(i); 	// need to check that these have been done	
	}	
	c1 = clock();  
	printf ("\telapsed CPU time: %f\n", (float) (c1 - c0)/CLOCKS_PER_SEC);	
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
