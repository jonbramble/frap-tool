#include "../include/tiffile.h" 

int Tiffile::getimagewidth(){
	return imagewidth;
}

int Tiffile::getimageheight(){
	return imageheight;
}	

tm Tiffile::gettm(){
	return imagetime;
}

char* Tiffile::getfilename(){
	return imagefilename;
}

Tiffile::Tiffile(char* filename){
	//std::cout << "constructor called" << std::endl;

	TIFF* tif = TIFFOpen(filename,"r");		//check for file here
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imagewidth);
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imageheight);

	//std::cout << "imagewidths" << std::endl;

	char* datetime = new char[4];
	TIFFGetField(tif, TIFFTAG_DATETIME, &datetime); 

	//std::cout << datetime << std::endl;
	
	char* pch[4];			// does the memory need to be allocated?
	char delims[] = " ";
	char *date,*fulltime,*shorttime,*meridian;
	  
	//issue with image pro express means datetime is trimmed by tifflib because 27 bytes long
	pch[0] = strtok(datetime, delims);			//chopping into sections with space delimiters recovers the data
	pch[1] = strtok(NULL, delims);
	pch[2] = strtok(NULL, delims);
		
	date = pch[0];
	fulltime = pch[1];
	meridian = pch[2];

	shorttime = strtok(fulltime, "."); // get and hence remove millisec portion of datetime
	ms = atoi(strtok(NULL, ".")); // convert to separate integer

	char to_parse[30];
	strcpy(to_parse,date);
	strcat(to_parse," ");
	strcat(to_parse,shorttime);
	strcat(to_parse," ");
	strcat(to_parse,meridian);
		
	strptime (to_parse, "%m/%d/%Y %r", &imagetime); //use ctime function to form date time structure

	time_t result = mktime(&imagetime);
	seconds = (double)(long(result)+(ms/1000.0));

	imagefilename = new char[sizeof(filename)];		//allocate memory to imagefilename!
	strcpy(imagefilename, filename);
	
	TIFFClose(tif);
	//std::cout << "File closed" << std::endl;
}
