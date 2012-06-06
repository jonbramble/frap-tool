/*
 * tiffile.cc
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
	std::cout << "Processing Image File...";
        std::cout << ".." << filename << "...";		//output file information

        TIFFErrorHandler handler;
	handler = TIFFSetWarningHandler(NULL);	//implement warning handler to supress ALL errors due to image pro express
	
	TIFF* tif = TIFFOpen(filename,"r");  //should check for file here
	TIFFSetWarningHandler(handler);
	
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imagewidth);  //get image scales
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imageheight);

	char* datetime = new char[4];
	TIFFGetField(tif, TIFFTAG_DATETIME, &datetime); 

	std::cout << datetime;
	
	char* pch[4];			// does the memory need to be allocated?
	char delims[] = " ";
	char *date,*fulltime;
	  
	//issue with image pro express means datetime is trimmed by tifflib because 27 bytes long
	pch[0] = strtok(datetime, delims);			//chopping into sections with space delimiters recovers the data
	pch[1] = strtok(NULL, delims);
		
	date = pch[0];
	fulltime = pch[1];


	//shorttime = strtok(fulltime, "."); // get and hence remove millisec portion of datetime
	//ms = atoi(strtok(NULL, ".")); // convert to separate integer
	ms = 0;

	char to_parse[30];
	strcpy(to_parse,date);
	strcat(to_parse," ");
	strcat(to_parse,fulltime);
	
	strptime (to_parse, "%m/%d/%Y %H:%M:%S", &imagetime); //use ctime function to form date time structure

	time_t result = mktime(&imagetime);
	seconds = (double)(long(result)+(ms/1000.0));

	imagefilename = new char[sizeof(filename)];		//allocate memory to imagefilename!
	strcpy(imagefilename, filename);
	
	TIFFClose(tif);
	std::cout << "...complete" << std::endl;
}
